#include "product.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

// ==================== Product����ʵ�� ====================

Product::Product(int id, const std::string& name, double price, int stock,
    const std::string& merchant, double discount)
    : productId(id), name(name), price(price), stock(stock),
    merchantName(merchant), discount(discount), frozenStock(0) {
    if (price < 0) {
        throw std::invalid_argument("��Ʒ�۸���Ϊ����");
    }
    if (stock < 0) {
        throw std::invalid_argument("��Ʒ��治��Ϊ����");
    }
    if (discount < 0.0 || discount > 1.0) {
        throw std::invalid_argument("�ۿ۱�����0.0��1.0֮��");
    }
}

double Product::getPrice() const {
    return price * discount;
}

void Product::setDiscount(double newDiscount) {
    if (newDiscount < 0.0 || newDiscount > 1.0) {
        throw std::invalid_argument("�ۿ۱�����0.0��1.0֮��");
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
        throw std::runtime_error("�ļ�δ��");
    }

    // д����Ʒ���ͣ����ڷ����л�ʱȷ���������ͣ�
    std::string type = getProductType();
    uint32_t typeLen = static_cast<uint32_t>(type.length());
    out.write(reinterpret_cast<const char*>(&typeLen), sizeof(typeLen));
    if (typeLen > 0) {
        out.write(type.c_str(), typeLen);
    }

    // д����ƷID
    out.write(reinterpret_cast<const char*>(&productId), sizeof(productId));

    // д����Ʒ����
    uint32_t nameLen = static_cast<uint32_t>(name.length());
    out.write(reinterpret_cast<const char*>(&nameLen), sizeof(nameLen));
    if (nameLen > 0) {
        out.write(name.c_str(), nameLen);
    }

    // д����Ʒ�۸�ԭ�ۣ�
    out.write(reinterpret_cast<const char*>(&price), sizeof(price));

    // д����Ʒ���
    out.write(reinterpret_cast<const char*>(&stock), sizeof(stock));

    // д������̼�
    uint32_t merchantLen = static_cast<uint32_t>(merchantName.length());
    out.write(reinterpret_cast<const char*>(&merchantLen), sizeof(merchantLen));
    if (merchantLen > 0) {
        out.write(merchantName.c_str(), merchantLen);
    }

    // д���ۿ�
    out.write(reinterpret_cast<const char*>(&discount), sizeof(discount));

    // д�붳���棨�����ֶΣ�
    out.write(reinterpret_cast<const char*>(&frozenStock), sizeof(frozenStock));
}

void Product::deserialize(std::ifstream& in) {
    if (!in.is_open()) {
        throw std::runtime_error("�ļ�δ��");
    }

    // ��ȡ��Ʒ����
    uint32_t typeLen;
    in.read(reinterpret_cast<char*>(&typeLen), sizeof(typeLen));
    if (in.fail() || typeLen > 1000) {
        throw std::runtime_error("��ȡ��Ʒ���ͳ���ʧ�ܻ򳤶��쳣: " + std::to_string(typeLen));
    }

    std::string type;
    if (typeLen > 0) {
        type.resize(typeLen);
        in.read(&type[0], typeLen);
        if (in.fail()) {
            throw std::runtime_error("��ȡ��Ʒ����ʧ��");
        }
    }

    // ��ȡ��ƷID
    in.read(reinterpret_cast<char*>(&productId), sizeof(productId));
    if (in.fail()) {
        throw std::runtime_error("��ȡ��ƷIDʧ��");
    }

    // ��ȡ��Ʒ����
    uint32_t nameLen;
    in.read(reinterpret_cast<char*>(&nameLen), sizeof(nameLen));
    if (in.fail() || nameLen > 1000) {
        throw std::runtime_error("��ȡ��Ʒ���Ƴ���ʧ�ܻ򳤶��쳣: " + std::to_string(nameLen));
    }
    if (nameLen > 0) {
        name.resize(nameLen);
        in.read(&name[0], nameLen);
        if (in.fail()) {
            throw std::runtime_error("��ȡ��Ʒ����ʧ��");
        }
    }
    else {
        name.clear();
    }

    // ��ȡ��Ʒ�۸�ԭ�ۣ�
    in.read(reinterpret_cast<char*>(&price), sizeof(price));
    if (in.fail()) {
        throw std::runtime_error("��ȡ��Ʒ�۸�ʧ��");
    }

    // ��ȡ��Ʒ���
    in.read(reinterpret_cast<char*>(&stock), sizeof(stock));
    if (in.fail()) {
        throw std::runtime_error("��ȡ��Ʒ���ʧ��");
    }

    // ��ȡ�����̼�
    uint32_t merchantLen;
    in.read(reinterpret_cast<char*>(&merchantLen), sizeof(merchantLen));
    if (in.fail() || merchantLen > 1000) {
        throw std::runtime_error("��ȡ�̼����Ƴ���ʧ�ܻ򳤶��쳣: " + std::to_string(merchantLen));
    }
    if (merchantLen > 0) {
        merchantName.resize(merchantLen);
        in.read(&merchantName[0], merchantLen);
        if (in.fail()) {
            throw std::runtime_error("��ȡ�̼�����ʧ��");
        }
    }
    else {
        merchantName.clear();
    }

    // ��ȡ�ۿ�
    in.read(reinterpret_cast<char*>(&discount), sizeof(discount));
    if (in.fail()) {
        throw std::runtime_error("��ȡ��Ʒ�ۿ�ʧ��");
    }

    // ��ȡ�����棨�����ֶΣ�
    in.read(reinterpret_cast<char*>(&frozenStock), sizeof(frozenStock));
    if (in.fail()) {
        throw std::runtime_error("��ȡ������ʧ��");
    }
}

std::string Product::toString() const {
    std::ostringstream oss;
    oss << "[" << productId << "] " << name
        << " - " << std::fixed << std::setprecision(2) << getPrice() << "Ԫ";

    // ֻ�����ۿ�ʱ��ʾ�ۿ���Ϣ
    if (hasDiscount()) {
        oss << " (ԭ��:" << price << "Ԫ, " << static_cast<int>(discount * 100) << "��)";
    }

    oss << " (���:" << stock << ") [" << merchantName << "] {" << getProductType() << "}";
    return oss.str();
}

std::string Product::toDetailString() const {
    std::ostringstream oss;
    oss << "��ƷID: " << productId << "\n"
        << "��Ʒ����: " << name << "\n"
        << "��Ʒ����: " << getProductType() << "\n";

    if (hasDiscount()) {
        oss << "ԭ��: " << std::fixed << std::setprecision(2) << price << " Ԫ\n"
            << "�ۿ�: " << static_cast<int>(discount * 100) << "��\n"
            << "�ּ�: " << getPrice() << " Ԫ\n";
    }
    else {
        oss << "�۸�: " << std::fixed << std::setprecision(2) << getPrice() << " Ԫ\n";
    }

    oss << "�������: " << stock << "\n"
        << "�����̼�: " << merchantName;

    return oss.str();
}

// ==================== Food��ʵ�� ====================

Food::Food(int id, const std::string& name, double price, int stock,
    const std::string& merchant, double discount)
    : Product(id, name, price, stock, merchant, discount) {
}

double Food::getPrice() const {
    // ʳƷ�����ʵ������ļ۸�������
    // ����򵥵��û����ʵ��
    return Product::getPrice();
}

// ==================== Book��ʵ�� ====================

Book::Book(int id, const std::string& name, double price, int stock,
    const std::string& merchant, double discount)
    : Product(id, name, price, stock, merchant, discount) {
}

double Book::getPrice() const {
    // �鼮�����ʵ������ļ۸�������
    // ���磺�������鼮�����ж����ۿ�
    return Product::getPrice();
}

// ==================== Clothing��ʵ�� ====================

Clothing::Clothing(int id, const std::string& name, double price, int stock,
    const std::string& merchant, double discount)
    : Product(id, name, price, stock, merchant, discount) {
}

double Clothing::getPrice() const {
    // �·������ʵ������ļ۸�������
    // ���磺�������ۿ�
    return Product::getPrice();
}