-- 用户表
CREATE TABLE IF NOT EXISTS users (
    username TEXT PRIMARY KEY,
    password_hash TEXT NOT NULL,
    user_type TEXT NOT NULL CHECK(user_type IN ('Consumer', 'Seller')),
    balance REAL DEFAULT 0.0 CHECK(balance >= 0)
);

-- 商品表
CREATE TABLE IF NOT EXISTS products (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    category TEXT NOT NULL,
    description TEXT,
    base_price REAL NOT NULL CHECK(base_price >= 0),
    quantity INTEGER NOT NULL CHECK(quantity >= 0),
    seller_username TEXT,
    FOREIGN KEY(seller_username) REFERENCES users(username)
);

-- 购物车表
CREATE TABLE IF NOT EXISTS cart_items (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_username TEXT,
    product_id INTEGER,
    quantity INTEGER NOT NULL CHECK(quantity > 0),
    FOREIGN KEY(user_username) REFERENCES users(username),
    FOREIGN KEY(product_id) REFERENCES products(id)
);

-- 订单表
CREATE TABLE IF NOT EXISTS orders (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_username TEXT,
    total_amount REAL NOT NULL,
    status TEXT CHECK(status IN ('pending', 'paid', 'cancelled')),
    created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(user_username) REFERENCES users(username)
);

-- 订单项目表
CREATE TABLE IF NOT EXISTS order_items (
    order_id INTEGER,
    product_id INTEGER,
    quantity INTEGER NOT NULL,
    price_at_time REAL NOT NULL,
    FOREIGN KEY(order_id) REFERENCES orders(id),
    FOREIGN KEY(product_id) REFERENCES products(id),
    PRIMARY KEY(order_id, product_id)
);