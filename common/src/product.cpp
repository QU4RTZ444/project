#include "product.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

// ==================== Product基类实现 ====================

Product::Product(int id, const std::string& name, double price, int stock,
    const std::string& merchant, double discount)
    : productId(id), name(name), price(price), stock(stock),
    merchantName(merchant), discount(discount), frozenStock(0) {
    if (price < 0) {
        throw std::invalid_argument("商品价格不能为负数");
    }
    if (stock < 0) {
        throw std::invalid_argument("商品库存不能为负数");
    }
    if (discount < 0.0 || discount > 1.0) {
        throw std::invalid_argument("折扣必须在0.0到1.0之间");
    }
}

double Product::getPrice() const {
    return price * discount;
}

void Product::setDiscount(double newDiscount) {
    if (newDiscount < 0.0 || newDiscount > 1.0) {
        throw std::invalid_argument("折扣必须在0.0到1.0之间");
    }
    discount = newDiscount;
}

bool Product::reduceStock(int quantity) {
    if (quantity <= 0 || quantity > (stock - frozenStock)) {
        return false;
    }
    stock -= quantity;
    return true;
}

bool Product::increaseStock(int quantity) {
    if (quantity <= 0) {
        return false;
    }
    stock += quantity;
    return true;
}

bool Product::isAvailable(int quantity) const {
    return quantity > 0 && quantity <= (stock - frozenStock);
}

bool Product::freezeStock(int quantity) {
    if (quantity <= 0 || quantity > (stock - frozenStock)) {
        return false;
    }
    frozenStock += quantity;
    return true;
}

bool Product::unfreezeStock(int quantity) {
    if (quantity <= 0 || quantity > frozenStock) {
        return false;
    }
    frozenStock -= quantity;
    return true;
}

void Product::serialize(std::ofstream& out) const {
    if (!out.is_open()) {
        throw std::runtime_error("文件未打开");
    }

    // 写入商品类型（用于反序列化时确定具体类型）
    std::string type = getProductType();
    uint32_t typeLen = static_cast<uint32_t>(type.length());
    out.write(reinterpret_cast<const char*>(&typeLen), sizeof(typeLen));
    if (typeLen > 0) {
        out.write(type.c_str(), typeLen);
    }

    // 写入商品ID
    out.write(reinterpret_cast<const char*>(&productId), sizeof(productId));

    // 写入商品名称
    uint32_t nameLen = static_cast<uint32_t>(name.length());
    out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    if (nameLen > 0) {
        out.write(name.c_str(), nameLen);
    }

    // 写入商品价格（原价）
    out.write(reinterpret_cast<const char*>(&price), sizeof(price));

    // 写入商品库存
    out.write(reinterpret_cast<const char*>(&stock), sizeof(stock));

    // 写入出售商家
    uint32_t merchantLen = static_cast<uint32_t>(merchantName.length());
    out.write(reinterpret_cast<const char*>(&merchantLen), sizeof(merchantLen));
    if (merchantLen > 0) {
        out.write(merchantName.c_str(), merchantLen);
    }

    // 写入折扣
    out.write(reinterpret_cast<const char*>(&discount), sizeof(discount));

    // 写入冻结库存（额外字段）
    out.write(reinterpret_cast<const char*>(&frozenStock), sizeof(frozenStock));
}

void Product::deserialize(std::ifstream& in) {
    if (!in.is_open()) {
        throw std::runtime_error("文件未打开");
    }

    // 读取商品类型
    uint32_t typeLen;
    in.read(reinterpret_cast<char*>(&typeLen), sizeof(typeLen));
    if (in.fail() || typeLen > 1000) {
        throw std::runtime_error("读取商品类型长度失败或长度异常: " + std::to_string(typeLen));
    }

    std::string type;
    if (typeLen > 0) {
        type.resize(typeLen);
        in.read(&type[0], typeLen);
        if (in.fail()) {
            throw std::runtime_error("读取商品类型失败");
        }
    }

    // 读取商品ID
    in.read(reinterpret_cast<char*>(&productId), sizeof(productId));
    if (in.fail()) {
        throw std::runtime_error("读取商品ID失败");
    }

    // 读取商品名称
    uint32_t nameLen;
    in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    if (in.fail() || nameLen > 1000) {
        throw std::runtime_error("读取商品名称长度失败或长度异常: " + std::to_string(nameLen));
    }
    if (nameLen > 0) {
        name.resize(nameLen);
        in.read(&name[0], nameLen);
        if (in.fail()) {
            throw std::runtime_error("读取商品名称失败");
        }
    }
    else {
        name.clear();
    }

    // 读取商品价格（原价）
    in.read(reinterpret_cast<char*>(&price), sizeof(price));
    if (in.fail()) {
        throw std::runtime_error("读取商品价格失败");
    }

    // 读取商品库存
    in.read(reinterpret_cast<char*>(&stock), sizeof(stock));
    if (in.fail()) {
        throw std::runtime_error("读取商品库存失败");
    }

    // 读取出售商家
    uint32_t merchantLen;
    in.read(reinterpret_cast<char*>(&merchantLen), sizeof(merchantLen));
    if (in.fail() || merchantLen > 1000) {
        throw std::runtime_error("读取商家名称长度失败或长度异常: " + std::to_string(merchantLen));
    }
    if (merchantLen > 0) {
        merchantName.resize(merchantLen);
        in.read(&merchantName[0], merchantLen);
        if (in.fail()) {
            throw std::runtime_error("读取商家名称失败");
        }
    }
    else {
        merchantName.clear();
    }

    // 读取折扣
    in.read(reinterpret_cast<char*>(&discount), sizeof(discount));
    if (in.fail()) {
        throw std::runtime_error("读取商品折扣失败");
    }

    // 读取冻结库存（额外字段）
    in.read(reinterpret_cast<char*>(&frozenStock), sizeof(frozenStock));
    if (in.fail()) {
        throw std::runtime_error("读取冻结库存失败");
    }
}

std::string Product::toString() const {
    std::ostringstream oss;
    oss << "[" << productId << "] " << name
        << " - " << std::fixed << std::setprecision(2) << getPrice() << "元";

    // 只在有折扣时显示折扣信息
    if (hasDiscount()) {
        oss << " (原价:" << price << "元, " << static_cast<int>(discount * 100) << "折)";
    }

    oss << " (库存:" << stock << ") [" << merchantName << "] {" << getProductType() << "}";
    return oss.str();
}

std::string Product::toDetailString() const {
    std::ostringstream oss;
    oss << "商品ID: " << productId << "\n"
        << "商品名称: " << name << "\n"
        << "商品类型: " << getProductType() << "\n";

    if (hasDiscount()) {
        oss << "原价: " << std::fixed << std::setprecision(2) << price << " 元\n"
            << "折扣: " << static_cast<int>(discount * 100) << "折\n"
            << "现价: " << getPrice() << " 元\n";
    }
    else {
        oss << "价格: " << std::fixed << std::setprecision(2) << getPrice() << " 元\n";
    }

    oss << "库存数量: " << stock << "\n"
        << "出售商家: " << merchantName;

    return oss.str();
}

// ==================== Food类实现 ====================

Food::Food(int id, const std::string& name, double price, int stock,
    const std::string& merchant, double discount)
    : Product(id, name, price, stock, merchant, discount) {
}

double Food::getPrice() const {
    // 食品类可以实现特殊的价格计算规则
    // 这里简单调用基类的实现
    return Product::getPrice();
}

// ==================== Book类实现 ====================

Book::Book(int id, const std::string& name, double price, int stock,
    const std::string& merchant, double discount)
    : Product(id, name, price, stock, merchant, discount) {
}

double Book::getPrice() const {
    // 书籍类可以实现特殊的价格计算规则
    // 例如：教育类书籍可能有额外折扣
    return Product::getPrice();
}

// ==================== Clothing类实现 ====================

Clothing::Clothing(int id, const std::string& name, double price, int stock,
    const std::string& merchant, double discount)
    : Product(id, name, price, stock, merchant, discount) {
}

double Clothing::getPrice() const {
    // 衣服类可以实现特殊的价格计算规则
    // 例如：季节性折扣
    return Product::getPrice();
}