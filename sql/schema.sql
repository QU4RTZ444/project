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
    category TEXT NOT NULL CHECK(category IN ('图书', '食品', '服装')),
    description TEXT,
    price REAL NOT NULL CHECK(price >= 0),
    quantity INTEGER NOT NULL CHECK(quantity >= 0),
    seller_username TEXT,
    discount_rate REAL DEFAULT 1.0 CHECK(discount_rate > 0 AND discount_rate <= 1),
    FOREIGN KEY(seller_username) REFERENCES users(username)
);

-- 购物车表
CREATE TABLE IF NOT EXISTS cart_items (
    username TEXT NOT NULL,
    product_id INTEGER NOT NULL,
    quantity INTEGER NOT NULL CHECK(quantity > 0),
    PRIMARY KEY (username, product_id),
    FOREIGN KEY(username) REFERENCES users(username),
    FOREIGN KEY(product_id) REFERENCES products(id)
);

-- 订单表
CREATE TABLE IF NOT EXISTS orders (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    buyer_username TEXT NOT NULL,
    total_amount REAL NOT NULL CHECK(total_amount > 0),
    status TEXT NOT NULL CHECK(status IN ('pending', 'paid', 'cancelled', 'failed')),
    create_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY(buyer_username) REFERENCES users(username)
);

-- 订单项目表
CREATE TABLE IF NOT EXISTS order_items (
    order_id INTEGER NOT NULL,
    product_id INTEGER NOT NULL,
    quantity INTEGER NOT NULL CHECK(quantity > 0),
    price REAL NOT NULL CHECK(price >= 0),
    seller_username TEXT NOT NULL,
    PRIMARY KEY (order_id, product_id),
    FOREIGN KEY(order_id) REFERENCES orders(id),
    FOREIGN KEY(product_id) REFERENCES products(id),
    FOREIGN KEY(seller_username) REFERENCES users(username)
);

-- 商品库存锁定表
CREATE TABLE IF NOT EXISTS product_locks (
    product_id INTEGER NOT NULL,
    order_id INTEGER NOT NULL,
    locked_quantity INTEGER NOT NULL CHECK(locked_quantity > 0),
    lock_time DATETIME DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (product_id, order_id),
    FOREIGN KEY(product_id) REFERENCES products(id),
    FOREIGN KEY(order_id) REFERENCES orders(id)
);