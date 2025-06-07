#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>
#include <fstream>
#include <vector>

/**
 * @brief ��Ʒ���ࣨ�����ࣩ
 * ������Ʒ�Ļ�����Ϣ�����ơ��۸񡢿�桢�̼ҡ��ۿ�
 */
class Product {
protected:
    int productId;
    std::string name;
    double price;              // ԭ��
    int stock;
    std::string merchantName;
    double discount;           // �ۿ�
    int frozenStock;           // �����棨Ϊ��������Ԥ����

public:
    /**
     * @brief ��Ʒ���๹�캯��
     * @param id ��ƷID
     * @param name ��Ʒ����
     * @param price ��Ʒ�۸�
     * @param stock ��Ʒ���
     * @param merchant �����̼�
     * @param discount �ۿۣ�Ĭ��1.0���ۿۣ�
     */
    Product(int id, const std::string& name, double price, int stock,
        const std::string& merchant, double discount = 1.0);

    virtual ~Product() = default;

    // Getter����
    int getProductId() const { return productId; }
    std::string getName() const { return name; }
    double getOriginalPrice() const { return price; }  // ��ȡԭ��
    virtual double getPrice() const;                   // ��ȡ�ּۣ������ۿۣ�
    int getStock() const { return stock; }
    std::string getMerchantName() const { return merchantName; }
    double getDiscount() const { return discount; }
    int getFrozenStock() const { return frozenStock; } // ��ȡ������
    virtual std::string getProductType() const = 0;

    // Setter����
    void setPrice(double newPrice) { price = newPrice; }
    void setStock(int newStock) { stock = newStock; }
    void setDiscount(double newDiscount);

    // ������
    bool reduceStock(int quantity);
    bool increaseStock(int quantity);

    // ���������Ϊ��������Ԥ����
    bool freezeStock(int quantity);
    bool unfreezeStock(int quantity);

    // ���л�����
    virtual void serialize(std::ofstream& out) const;
    virtual void deserialize(std::ifstream& in);

    // ��ʽ����ʾ
    std::string toString() const;
    std::string toDetailString() const;

    // Ϊ���׹���Ԥ��������Ƿ�ɹ���ָ������
    bool isAvailable(int quantity) const;

    // ����Ƿ����ۿ�
    bool hasDiscount() const { return discount < 1.0; }
};

/**
 * @brief ʳƷ��
 * �̳���Product����
 */
class Food : public Product {
public:
    /**
     * @brief ʳƷ�๹�캯��
     */
    Food(int id, const std::string& name, double price, int stock,
        const std::string& merchant, double discount = 1.0);

    /**
     * @brief ʵ�ִ��麯�� - ������Ʒ����
     * @return "ʳƷ"
     */
    std::string getProductType() const override { return "ʳƷ"; }

    /**
     * @brief ��д�۸���㣨ʳƷ�����������ۿ۹���
     * @return ʵ�ʼ۸�
     */
    double getPrice() const override;
};

/**
 * @brief �鼮��
 * �̳���Product����
 */
class Book : public Product {
public:
    /**
     * @brief �鼮�๹�캯��
     */
    Book(int id, const std::string& name, double price, int stock,
        const std::string& merchant, double discount = 1.0);

    /**
     * @brief ʵ�ִ��麯�� - ������Ʒ����
     * @return "�鼮"
     */
    std::string getProductType() const override { return "�鼮"; }

    /**
     * @brief ��д�۸���㣨�鼮�����н����ۿ۵ȣ�
     * @return ʵ�ʼ۸�
     */
    double getPrice() const override;
};

/**
 * @brief �·���
 * �̳���Product����
 */
class Clothing : public Product {
public:
    /**
     * @brief �·��๹�캯��
     */
    Clothing(int id, const std::string& name, double price, int stock,
        const std::string& merchant, double discount = 1.0);

    /**
     * @brief ʵ�ִ��麯�� - ������Ʒ����
     * @return "�·�"
     */
    std::string getProductType() const override { return "�·�"; }

    /**
     * @brief ��д�۸���㣨�·������м������ۿ۵ȣ�
     * @return ʵ�ʼ۸�
     */
    double getPrice() const override;
};

#endif