import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.security import check_password_hash, generate_password_hash
from datetime import datetime

from helpers import apology, login_required, lookup, usd

# Configure application
app = Flask(__name__)

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")

# Make sure API key is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")


@app.after_request
def after_request(response):
    """Ensure responses aren't cached"""
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response


@app.route("/", methods=["GET", "POST"])
@app.route("/index", methods = ["GET", "POST"])
@login_required
def index():
    # Get user data from db
    user_stocks = db.execute("SELECT stock, SUM(amount) FROM transactions WHERE buyer = ? GROUP BY stock", session["user_id"])
    user_cash = db.execute("SELECT * FROM users WHERE id = ?", session["user_id"])
    current_worth = 0

    # Asign variables to show in HTML table
    for stock in user_stocks:
        stock_data = lookup(stock["stock"])
        stock["currentprice"] = stock_data["price"]
        stock["totalprice"] = stock_data["price"] * stock["SUM(amount)"]
        current_worth += stock["totalprice"]
    return render_template("index.html", user_stocks = user_stocks, current_worth = current_worth, user_cash = user_cash)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    if request.method == "GET":
        return render_template("buy.html")

    if request.method == "POST":

        # Get symbol and look up its price
        symbol = request.form.get("symbol")
        if (lookup(symbol) is None):
            return apology("invalid symbol")

        # Get purchase amount, check validity
        number = request.form.get("shares")
        if (number.isnumeric() is False):
            return apology("THAT IS NOT A POSITIVE INTEGER")
        if (int(number)<=0):
            return apology("YOU CANT BUY 0 OF SOMETHING YOU FOOL")

        # Get user money for purchase validation
        user_money = db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])
        user_money = user_money[0]["cash"]

        # Get information for the stock trying to be bought
        data = lookup(symbol)
        price = data["price"]
        shares_price = float(number) * float(price)
        if (user_money < shares_price):
            return apology("not enough moolah")

        # If here, buy was successful. Update user cash and insert purchase info into DB
        user_money-=shares_price
        shares_f = "{:.2f}".format(shares_price)
        shares_f = str(shares_f)
        db.execute("INSERT INTO transactions (buyer, stock, amount, buy_price, date, type) VALUES (?, ?, ?, ?, ?, ?)", session["user_id"], symbol, number, price, datetime.today(), "buy")
        db.execute("UPDATE users SET cash = ? WHERE id = ?", user_money, session["user_id"])

        # After all is done, redirect to main page
        return redirect("/")



@app.route("/history", methods = ["GET", "POST"])
@login_required
def history():
    if request.method == "GET":

        # Get transactions from db to show in table
        transactions = db.execute("SELECT * FROM transactions WHERE buyer = ? ORDER BY DATE DESC", session["user_id"])
        for transaction in transactions:
            transaction["buy_price"] = transaction["buy_price"] * transaction["amount"]
        return render_template ("history.html", transactions = transactions)


@app.route("/login", methods=["GET", "POST"])
def login():
    """Log user in"""

    # Forget any user_id
    session.clear()

    # User reached route via POST (as by submitting a form via POST)
    if request.method == "POST":

        # Ensure username was submitted
        if not request.form.get("username"):
            return apology("must provide username", 403)

        # Ensure password was submitted
        elif not request.form.get("password"):
            return apology("must provide password", 403)

        # Query database for username
        rows = db.execute("SELECT * FROM users WHERE username = ?", request.form.get("username"))

        # Ensure username exists and password is correct
        if len(rows) != 1 or not check_password_hash(rows[0]["hash"], request.form.get("password")):
            return apology("invalid username and/or password", 403)

        # Remember which user has logged in
        session["user_id"] = rows[0]["id"]

        # Redirect user to home page
        return redirect("/")

    # User reached route via GET (as by clicking a link or via redirect)
    else:
        return render_template("login.html")


@app.route("/logout")
def logout():
    """Log user out"""

    # Forget any user_id
    session.clear()

    # Redirect user to login form
    return redirect("/")


@app.route("/quote", methods=["GET", "POST"])
@login_required
def quote():

    if request.method == "GET":
        return render_template("quote.html")

    if request.method =="POST":
        # Get symbol from form, show price
        symbol = request.form.get("symbol")
        symbol=lookup(symbol)
        if symbol is None:
            return apology("Stock does not exist")
        return render_template("quoted.html", symbol=symbol)


@app.route("/register", methods=["GET", "POST"])
def register():

    if request.method == "GET":
        return render_template ("register.html")

    if request.method == "POST":

        # Get user info from form
        username = request.form.get("username")
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")
        used_usernames = db.execute("SELECT username FROM users")

        # Check for username and password validity
        if not username or not password or not confirmation:
            return apology("NO BLANK FIELDS PLEASE")

        for used in used_usernames:
            if username in used["username"]:
                return apology("This user already exists")

        if password != confirmation:
            return apology("password and confirmation dont match")

        # If here, password and username are valid. Hash password and insert values into DB
        hash = generate_password_hash(password)
        db.execute("INSERT INTO users (username, hash) VALUES (?, ?)", username, hash)
        return redirect("/")





@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():

    if request.method == "GET":

        # Get stocks the user currently owns from DB
        user_stocks = db.execute("SELECT stock FROM transactions WHERE buyer = ? GROUP BY stock", session["user_id"])
        return render_template("sell.html", user_stocks = user_stocks)

    if request.method == "POST":

        # Get user stocks, money and information about the stock to be sold
        user_stocks = db.execute("SELECT stock, SUM(amount) FROM transactions WHERE buyer = ? GROUP BY stock", session["user_id"])
        sell_amount = request.form.get("shares")
        sell_symbol = request.form.get("symbol")
        stock_data = lookup(sell_symbol)
        current_price = stock_data["price"]
        user_money = db.execute("SELECT cash FROM users WHERE id = ?", session["user_id"])
        user_money = user_money[0]["cash"]

        # Look for the stock being sold in the user stocks. If the user has enough, sell them
        for stock in user_stocks:
            if stock["stock"] == sell_symbol:
                if int(sell_amount) > stock["SUM(amount)"]:
                    return apology("not enough shares")
                if ((sell_amount).isnumeric() is False):
                    return apology("THAT IS NOT A POSITIVE INTEGER")

                # If here, sale is valid. Update user money and record transaction
                user_money += (float(sell_amount) * float(current_price))
                sell_amount = int(sell_amount) * (-1)
                db.execute("UPDATE users SET cash = ? WHERE id = ?", user_money, session["user_id"])
                db.execute("INSERT INTO transactions (buyer, stock, amount, buy_price, date, type) VALUES (?, ?, ?, ?, ?, ?)", session["user_id"], sell_symbol, int(sell_amount), current_price, datetime.today(), "sell")

                return redirect("/")

        # Failsafe just in case the stock trying to be sold is not owned by user
        return apology("You do not own any of that stock")




