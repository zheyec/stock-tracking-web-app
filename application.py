import os

from cs50 import SQL
from flask import Flask, flash, redirect, render_template, request, session
from flask_session import Session
from tempfile import mkdtemp
from werkzeug.exceptions import default_exceptions
from werkzeug.security import check_password_hash, generate_password_hash

from helpers import apology, login_required, lookup, usd

# Ensure environment variable is set
if not os.environ.get("API_KEY"):
    raise RuntimeError("API_KEY not set")

# Configure application
app = Flask(__name__)

# Ensure templates are auto-reloaded
app.config["TEMPLATES_AUTO_RELOAD"] = True

# Ensure responses aren't cached
@app.after_request
def after_request(response):
    response.headers["Cache-Control"] = "no-cache, no-store, must-revalidate"
    response.headers["Expires"] = 0
    response.headers["Pragma"] = "no-cache"
    return response

# Custom filter
app.jinja_env.filters["usd"] = usd

# Configure session to use filesystem (instead of signed cookies)
app.config["SESSION_FILE_DIR"] = mkdtemp()
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_TYPE"] = "filesystem"
Session(app)

# Configure CS50 Library to use SQLite database
db = SQL("sqlite:///finance.db")


@app.route("/")
@login_required
def index():
    """Show portfolio of stocks"""
    table = db.execute("SELECT symbol, shares, curPrice FROM stocks WHERE id = :id", id=session["user_id"])
    sum = 0
    for row in table:
        row["curPrice"] = lookup(row["symbol"])["price"]
        sum += row["curPrice"] * row["shares"]
    return render_template("index.html", table=table, sum=sum)


@app.route("/buy", methods=["GET", "POST"])
@login_required
def buy():
    """Buy shares of stock"""

    if request.method == "POST":

        # Ensure symbol was submitted
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("must provide symbol", 400)

        # Ensure number of shares is a positive integer
        shares = request.form.get("shares")
        if not shares:
            return apology("must provide number of shares", 400)
        shareNum = int(shares);
        if shareNum <= 0:
            return apology("number of shares must be a positive integer", 400)

        # Get user's cash and check if purchase is affordable
        rows = db.execute("SELECT cash FROM users WHERE id = :id", id=session["user_id"])
        if len(rows) != 1:
            return apology("cannot access cash", 400)
        cash = int(rows[0]["cash"])
        price = lookup(symbol)["price"]
        cost = price * shareNum
        if cost > cash:
            return apology("out of cash", 400)

        # Update stocks (recording stocks owned by all users)
        rows = db.execute("SELECT * FROM stocks WHERE id = :id AND symbol = :symbol",
                            id=session["user_id"], symbol=symbol)
        # If user already has stocks, add shareNum to shares
        if len(rows) == 1:
            db.execute("UPDATE stocks SET shares=:shares WHERE id = :id AND symbol = :symbol",
                        shares=rows[0]["shares"] + shareNum, id=session["user_id"], symbol=symbol)
        # Add new record otherwise
        else:
            db.execute("INSERT INTO stocks (symbol, shares, id) VALUES (:symbol, :shares, :id)",
                        symbol=symbol, shares=shareNum, id=session["user_id"])

        # Update history (recording selling & buying histories)
        result = lookup(symbol)
        curDateTime = db.execute("SELECT datetime('now')")[0]["datetime(\'now\')"]
        db.execute("INSERT INTO history (symbol, isBuying, price, shares, datetime, id) VALUES (:symbol, :value, :price, :shares, :d, :id)",
                    symbol=symbol, value=1, price=result["price"], shares=shareNum, d=curDateTime, id=session["user_id"])

        # Update cash
        table = db.execute("SELECT * FROM users WHERE id = :id", id=session["user_id"])
        table[0]["cash"] -= cost

    # If request method is GET, return the page
    return render_template("buy.html")


@app.route("/history")
@login_required
def history():
    """Show history of transactions"""
    return render_template("history.html", table=db.execute("SELECT * FROM history"))


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
        rows = db.execute("SELECT * FROM users WHERE username = :username",
                          username=request.form.get("username"))

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
    """Get stock quote."""
    if request.method == "POST":

        # Ensure symbol was submitted
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("must provide symbol", 400)

        # Embed the return of lookup
        result = lookup(symbol)
        return render_template("quoted.html", symbol=result["symbol"], price=usd(result["price"]))

    # Render the page if method is GET
    return render_template("quote.html")


@app.route("/register", methods=["GET", "POST"])
def register():
    """Register user"""
    if request.method == "POST":

        # Ensure username was submitted
        username = request.form.get("username")
        if not username:
            return apology("must provide username", 400)

        # Ensure username is unique
        rows = db.execute("SELECT * FROM users WHERE username = :username", username=username)
        if len(rows) == 1:
            return apology("username already exists", 400)

        # Ensure password and password confirmation match
        password = request.form.get("password")
        confirmation = request.form.get("confirmation")
        if password != confirmation:
            return apology("passwords do not match", 400)
        if password == "":
            return apology("must provide password", 400)

        # Add user to database
        db.execute("INSERT INTO users (username, hash) VALUES (:username, :hsh)",
                    username=username, hsh=generate_password_hash(password))

        # Redirect user to home page
        return redirect("/")

    # Render the page if method is GET
    return render_template("register.html")


@app.route("/sell", methods=["GET", "POST"])
@login_required
def sell():
    """Sell shares of stock"""

    if request.method == "POST":

        # Ensure symbol was submitted
        symbol = request.form.get("symbol")
        if not symbol:
            return apology("must provide symbol", 403)

        # Ensure number of shares is a positive integer
        shares = request.form.get("shares")
        if not shares:
            return apology("must provide number of shares", 403)
        shareNum = int(shares);
        if shareNum <= 0:
            return apology("number of shares must be a positive integer", 403)

        # Get user's number of shares and check if selling is doable
        rows = db.execute("SELECT shares FROM stocks WHERE id = :id AND symbol = :symbol",
                            id=session["user_id"], symbol=symbol)
        if len(rows) != 1:
            return apology("no such stocks exist", 403)
        curShareNum = int(rows[0]["shares"])
        if curShareNum < shareNum:
            return apology("out of shares", 403)

        # Update stocks (recording stocks owned by all users)
        rows = db.execute("SELECT * FROM stocks WHERE id = :id AND symbol = :symbol",
                            id=session["user_id"], symbol=symbol)

        # Deduct the number of shares sold
        db.execute("UPDATE stocks SET shares=:shares WHERE id = :id AND symbol = :symbol",
                        shares=rows[0]["shares"] - shareNum, id=session["user_id"], symbol=symbol)

        # Delete the record if necessary
        if shareNum == curShareNum:
            db.execute("DELETE FROM stocks WHERE id = :id AND symbol = :symbol",
                        id=session["user_id"], symbol=symbol)

        # Update history (recording selling & buying histories)
        result = lookup(symbol)
        curDateTime = db.execute("SELECT datetime('now')")[0]["datetime(\'now\')"]
        db.execute("INSERT INTO history (symbol, isBuying, price, shares, datetime, id) VALUES (:symbol, :value, :price, :shares, :d, :id)",
                    symbol=symbol, value=0, price=result["price"], shares=shareNum, d=curDateTime, id=session["user_id"])

        # Update cash
        table = db.execute("SELECT * FROM users WHERE id = :id", id=session["user_id"])
        table[0]["cash"] += shareNum * lookup(symbol)["price"]

    # If request method is GET, return the page
    return render_template("sell.html")


def errorhandler(e):
    """Handle error"""
    return apology(e.name, e.code)


# listen for errors
for code in default_exceptions:
    app.errorhandler(code)(errorhandler)
