/*
 * An example file showing the usage of the C++ libmemcached interface.
 */

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#include <string.h>

#include <libmemcached/memcached.hpp>

using namespace std;
using namespace memcache;

class DeletePtrs
{
public:
  template<typename T>
  inline void operator()(const T *ptr) const
  {
    delete ptr;
  }
};

class MyCache
{
public:

  static const uint32_t num_of_clients= 10;

  static MyCache &singleton()
  {
    static MyCache instance;
    return instance;
  }

  void set(const string &key,
           const vector<char> &value)
  {
    time_t expiry= 0;
    uint32_t flags= 0;
    getCache()->set(key, value, expiry, flags);
  }

  vector<char> get(const string &key)
  {
    vector<char> ret_value;
    getCache()->get(key, ret_value);
    return ret_value;
  }

  void remove(const string &key)
  {
    getCache()->remove(key);
  }

  Memcache *getCache()
  {
    /* 
     * pick a random element from the vector of clients. Obviously, this is
     * not very random but suffices as an example!
     */
    uint32_t index= rand() % num_of_clients;
    return clients[index];
  } 

private:

  /*
   * A vector of clients.
   */
  std::vector<Memcache *> clients;

  MyCache()
    :
      clients()
  {
    /* create clients and add them to the vector */
    for (uint32_t i= 0; i < num_of_clients; i++)
    {
      Memcache *client= new Memcache("127.0.0.1:11211");
      clients.push_back(client);
    }
  }

  ~MyCache()
  {
    for_each(clients.begin(), clients.end(), DeletePtrs());
    clients.clear();
  }

  MyCache(const MyCache&);

};

class Product
{
public:

  Product(int in_id, double in_price)
    :
      id(in_id),
      price(in_price)
  {}

  Product()
    :
      id(0),
      price(0.0)
  {}

  int getId() const
  {
    return id;
  }

  double getPrice() const
  {
    return price;
  }

private:

  int id;
  double price;

};

void setAllProducts(vector<Product> &products)
{
  vector<char> raw_products(products.size() * sizeof(Product));
  memcpy(&raw_products[0], &products[0], products.size() * sizeof(Product));
  MyCache::singleton().set("AllProducts", raw_products);
}

vector<Product> getAllProducts()
{
  vector<char> raw_products = MyCache::singleton().get("AllProducts");
  vector<Product> products(raw_products.size() / sizeof(Product));
  memcpy(&products[0], &raw_products[0], raw_products.size());
  return products;
}

Product getProduct(const string &key)
{
  vector<char> raw_product= MyCache::singleton().get(key);
  Product ret;
  if (! raw_product.empty())
  {
    memcpy(&ret, &raw_product[0], sizeof(Product));
  }
  else
  {
    /* retrieve it from the persistent store */
  }
  return ret;
}

void setProduct(const string &key, const Product &product)
{
  vector<char> raw_product(sizeof(Product));
  memcpy(&raw_product[0], &product, sizeof(Product));
  MyCache::singleton().set(key, raw_product);
}

int main()
{
#if 0
  Product pad(1, 5.0);
  const string key("padraig");
  cout << "Going to set an object in the cache..." << endl;
  setProduct(key, pad);
  cout << "Now retrieve that key..." << endl;
  Product test= getProduct(key);
  double price= test.getPrice();
  cout << "Price of retrieve object: " << price << endl;
  Product next(2, 10.0);
  vector<Product> products;
  products.push_back(pad);
  products.push_back(next);
  cout << "going to set a vector of products..." << endl;
  setAllProducts(products);
  cout << "now retrieve those products..." << endl;
  vector<Product> got= getAllProducts();
  cout << "size of retrieved vector: " << got.size() << endl;
  vector<Product>::iterator iter= got.begin();
  while (iter != got.end())
  {
    cout << "product " << (*iter).getId() << " costs " << (*iter).getPrice() << endl;
    ++iter;
  }
#endif
  Memcache first_client("127.0.0.1:11211");
  Memcache second_client("127.0.0.1", 11211);
  //first_client.set("key", some_vector_of_chars, expiry, flags);
  //first_client.get("key", vector_to_fill_with_data);
  //first_client.remove("key");
  first_client.addServer("192.168.1.1", 11211);
  return 0;
}
