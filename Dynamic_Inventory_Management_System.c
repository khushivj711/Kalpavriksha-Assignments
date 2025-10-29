#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MIN_PRODUCTS 1
#define MAX_PRODUCTS 100
#define MIN_PRODUCT_ID 1
#define MAX_PRODUCT_ID 10000
#define MIN_PRICE 0
#define MAX_PRICE 100000
#define MIN_QUANTITY 0
#define MAX_QUANTITY 1000000

typedef struct
{
  int productId;
  char productName[MAX_NAME_LENGTH];
  float productPrice;
  int productQuality;
} ProductDetails;

bool isValidNumberOfProducts(int numberOfProducts);
bool isValidProductID(int id);
bool isValidPrice(float price);
bool isValidQuantity(int quantity);
bool isListEmpty(ProductDetails *productDetails, int numberOfProducts);
bool isDuplicateProductID(ProductDetails *productDetails, int numberOfProducts, int id);
bool isProductNameContains(const char *productName, const char *searchName);
void inputValidNumberOfProducts(int *numberOfProducts);
void displayDetailsOfProducts(ProductDetails *productDetails, int numberOfProducts);
void inventoryMenu();
void sortByProductID(ProductDetails *productDetails, int numberOfProducts);
void viewAllProducts(ProductDetails *productDetails, int numberOfProducts);
ProductDetails *addNewProduct(ProductDetails *productDetails, int *numberOfProducts);
void updateQuantityOfProduct(ProductDetails *productDetails, int numberOfProducts);
void searchProductByID(ProductDetails *productDetails, int numberOfProducts);
void searchProductByName(ProductDetails *productDetails, int numberOfProducts);
void searchProductByPriceRange(ProductDetails *productDetails, int numberOfProducts);
ProductDetails *deleteProductByID(ProductDetails *productDetails, int *numberOfProducts);
void processInventory(ProductDetails **productDetails, int *numberOfProducts);

int main()
{
  int numberOfProducts;
  inputValidNumberOfProducts(&numberOfProducts);
  ProductDetails *productDetails = (ProductDetails *)calloc(numberOfProducts, sizeof(ProductDetails));
  if (productDetails == NULL)
  {
    printf("Memory is not allocated!!");
    return 1;
  }

  displayDetailsOfProducts(productDetails, numberOfProducts);
  processInventory(&productDetails, &numberOfProducts);
  free(productDetails);
  productDetails = NULL;

  return 0;
}

bool isValidNumberOfProducts(int numberOfProducts)
{
  return (numberOfProducts >= 1 && numberOfProducts <= MAX_PRODUCTS);
}

bool isValidProductID(int id)
{
  return id >= MIN_PRODUCT_ID && id <= MAX_PRODUCT_ID;
}

bool isValidPrice(float price)
{
  return price >= MIN_PRICE && price <= MAX_PRICE;
}

bool isValidQuantity(int quantity)
{
  return quantity >= MIN_QUANTITY && quantity <= MAX_QUANTITY;
}

bool isListEmpty(ProductDetails *productDetails, int numberOfProducts)
{
  if (productDetails == NULL || numberOfProducts == 0)
  {
    printf("No products available.\n");
    return true;
  }
  return false;
}

bool isDuplicateProductID(ProductDetails *productDetails, int numberOfProducts, int id)
{
  for (int i = 0; i < numberOfProducts; i++)
  {
    if((productDetails + i)->productId == id)
    {
      return true;
    }
  }
  return false;  
}

bool isProductNameContains(const char *productName, const char *searchName)
{
  int lenMain = strlen(productName);
  int lenSub = strlen(searchName);

  if(lenSub == 0 || lenSub > lenMain)
  {
    return false;
  }

  for (int i = 0; i <= lenMain-lenSub; i++)
  {
    int j ;
    for (j = 0; j < lenSub; j++)
    {
      if(productName[i+j] != searchName[j])
      {
        break;
      }
    }
    if(j == lenSub)
    {
      return true;
    }
  }
  return false;
}

void inputValidNumberOfProducts(int *numberOfProducts)
{
  do
  {
    printf("Enter the initial number of products: ");
    scanf("%d", numberOfProducts);
    if (!isValidNumberOfProducts(*numberOfProducts))
    {
      printf("Invalid input! Please enter values between 1 and 100.\n");
    }
    else{
      break;
    }
  } while (!isValidNumberOfProducts(*numberOfProducts));
}

void displayDetailsOfProducts(ProductDetails *productDetails, int numberOfProducts)
{
  for (int i = 0; i < numberOfProducts; i++)
  {
    printf("Enter details for product %d:\n", i + 1);

    printf("Product ID: ");
    scanf("%d", &(productDetails + i)->productId);
    getchar();

    if (!isValidProductID((productDetails + i)->productId))
    {
      printf("Invalid Product ID! It must be between 1 and 10000.\n");
      return;
    }

    printf("Product Name: ");
    fgets((productDetails + i)->productName, MAX_NAME_LENGTH, stdin);
    (productDetails + i)->productName[strcspn((productDetails + i)->productName, "\n")] = '\0';

    printf("Product Price: ");
    scanf("%f", &(productDetails + i)->productPrice);
    if (!isValidPrice((productDetails + i)->productPrice))
    {
      printf("Invalid Price! It must be between 0 and 100000.\n");
      return;
    }

    printf("Product Quantity: ");
    scanf("%d", &(productDetails + i)->productQuality);
    if (!isValidQuantity((productDetails + i)->productQuality))
    {
      printf("Invalid Quantity! It must be between 0 and 1000000.\n");
      return;
    }

    printf("\n");
  }
}

void inventoryMenu()
{
  printf("\n");
  printf("========= INVENTORY MENU =========\n");
  printf("1. Add New Product\n");
  printf("2. View All Products\n");
  printf("3. Update Quantity\n");
  printf("4. Search Product by ID\n");
  printf("5. Search Product by Name\n");
  printf("6. Search Product by Price Range\n");
  printf("7. Delete Product\n");
  printf("8. Exit\n");
}

void sortByProductID(ProductDetails *productDetails, int numberOfProducts)
{
  for (int i = 0; i < numberOfProducts - 1; i++)
  {
    for (int j = 0; j < numberOfProducts - i - 1; j++)
    {
      if ((productDetails + j)->productId > (productDetails + j + 1)->productId)
      {
        ProductDetails temp = *(productDetails + j);
        *(productDetails + j) = *(productDetails + j + 1);
        *(productDetails + j + 1) = temp;
      }
    }
  }
}

void viewAllProducts(ProductDetails *productDetails, int numberOfProducts)
{
  if (isListEmpty(productDetails, numberOfProducts))
  {
    return;
  }

  sortByProductID(productDetails, numberOfProducts);

  printf("\n========= PRODUCT LIST =========\n");
  for (int i = 0; i < numberOfProducts; i++)
  {
    printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n", (productDetails + i)->productId, (productDetails + i)->productName, (productDetails + i)->productPrice, (productDetails + i)->productQuality);
  }
  printf("\n");
}

ProductDetails *addNewProduct(ProductDetails *productDetails, int *numberOfProducts)
{
  (*numberOfProducts)++;
  ProductDetails *newProductDetail = (ProductDetails *)realloc(productDetails, (*numberOfProducts) * sizeof(ProductDetails));

  if (newProductDetail == NULL)
  {
    printf("Memory reallocation failed!\n");
    return productDetails;
  }

  printf("Enter new product details: \n");

  printf("Product ID: ");
  scanf("%d", &newProductDetail[*numberOfProducts - 1].productId);
  getchar();

  if (!isValidProductID(newProductDetail[*numberOfProducts - 1].productId) || isDuplicateProductID(newProductDetail, *numberOfProducts - 1 , newProductDetail[*numberOfProducts - 1].productId))
  {
    printf("Invalid Product ID! It must be between 1 and 10000.\n");
    return productDetails;
  }

  printf("Product Name: ");
  fgets(newProductDetail[*numberOfProducts - 1].productName, MAX_NAME_LENGTH, stdin);
  newProductDetail[*numberOfProducts - 1].productName[strcspn(newProductDetail[*numberOfProducts - 1].productName, "\n")] = '\0';

  printf("Product Price: ");
  scanf("%f", &newProductDetail[*numberOfProducts - 1].productPrice);

  if (!isValidPrice(newProductDetail[*numberOfProducts - 1].productPrice))
  {
    printf("Invalid Price! It must be between 0 and 100000.\n");
    return productDetails;
  }

  printf("Product Quantity: ");
  scanf("%d", &newProductDetail[*numberOfProducts - 1].productQuality);

  if (!isValidQuantity(newProductDetail[*numberOfProducts - 1].productQuality))
  {
    printf("Invalid Quantity! It must be between 0 and 1000000.\n");
    return productDetails;
  }

  printf("Product added Successfully!\n");
  return newProductDetail;
}

void updateQuantityOfProduct(ProductDetails *productDetails, int numberOfProducts)
{
  if (isListEmpty(productDetails, numberOfProducts))
  {
    return;
  }

  int idToUpdate, newQuantity;
  bool found = false;
  printf("Enter Product ID to update quantity: ");
  scanf("%d", &idToUpdate);

  if (!isValidProductID(idToUpdate))
  {
    printf("Invalid Product ID! Must be between %d and %d.\n", MIN_PRODUCT_ID, MAX_PRODUCT_ID);
    return;
  }

  for (int i = 0; i < numberOfProducts; i++)
  {
    if ((productDetails + i)->productId == idToUpdate)
    {
      printf("Enter new Quantity: ");
      scanf("%d", &newQuantity);
      if (!isValidQuantity(newQuantity))
      {
        printf("Invalid quantity! Must be between %d and %d.\n", MIN_QUANTITY, MAX_QUANTITY);
        return;
      }
      (productDetails + i)->productQuality = newQuantity;
      printf("Quantity updated successfully.\n");
      found = true;
      break;
    }
  }

  if (!found)
  {
    printf("Product with ID %d not found!\n", idToUpdate);
  }
}

void searchProductByID(ProductDetails *productDetails, int numberOfProducts)
{
  if (isListEmpty(productDetails, numberOfProducts))
  {
    return;
  }

  int productId;
  printf("Enter Product ID to search: ");
  scanf("%d", &productId);

  sortByProductID(productDetails, numberOfProducts);

  for (int i = 0; i < numberOfProducts; i++)
  {
    if ((productDetails + i)->productId == productId)
    {
      printf("\nProduct Found:\n");
      printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n", (productDetails + i)->productId, (productDetails + i)->productName, (productDetails + i)->productPrice, (productDetails + i)->productQuality);
      return;
    }
  }
  printf("Product with ID %d not found.\n", productId);
}

void searchProductByName(ProductDetails *productDetails, int numberOfProducts)
{
  if (isListEmpty(productDetails, numberOfProducts))
  {
    return;
  }

  char searchName[MAX_NAME_LENGTH];
  printf("Enter name to search (partial allowed): ");
  getchar();
  fgets(searchName, MAX_NAME_LENGTH, stdin);
  searchName[strcspn(searchName, "\n")] = '\0';

  int found = 0;
  printf("Products Found:\n");

  for (int i = 0; i < numberOfProducts; i++)
  {
    if (isProductNameContains((productDetails + i)->productName, searchName))
    {
      printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n", (productDetails + i)->productId, (productDetails + i)->productName, (productDetails + i)->productPrice, (productDetails + i)->productQuality);
      found = 1;
    }
  }

  if (!found)
  {
    printf("No product found with name containing '%s'.\n", searchName);
  }
}

void searchProductByPriceRange(ProductDetails *productDetails, int numberOfProducts)
{
  if (isListEmpty(productDetails, numberOfProducts))
  {
    return;
  }

  float minPrice, maxPrice;
  printf("Enter minimum price: ");
  scanf("%f", &minPrice);
  printf("Enter maximum price: ");
  scanf("%f", &maxPrice);

  if (minPrice > maxPrice)
  {
    float temp = minPrice;
    minPrice = maxPrice;
    maxPrice = temp;
  }

  int found = 0;
  printf("Products in price range:\n");
  for (int i = 0; i < numberOfProducts; i++)
  {
    float price = (productDetails + i)->productPrice;
    if (price >= minPrice && price <= maxPrice)
    {
      printf("Product ID: %d | Name: %s | Price: %.2f | Quantity: %d\n", (productDetails + i)->productId, (productDetails + i)->productName, (productDetails + i)->productPrice, (productDetails + i)->productQuality);
      found = 1;
    }
  }
  if (!found)
  {
    printf("No products found in the given price range.\n");
  }
}

ProductDetails *deleteProductByID(ProductDetails *productDetails, int *numberOfProducts)
{
  if (isListEmpty(productDetails, *numberOfProducts))
  {
    return productDetails;
  }

  int deleteID, found = 0;
  printf("Enter Product ID to delete: ");
  scanf("%d", &deleteID);

  for (int i = 0; i < *numberOfProducts; i++)
  {
    if (productDetails[i].productId == deleteID)
    {
      for (int j = i; j < *numberOfProducts - 1; j++)
      {
        productDetails[j] = productDetails[j + 1];
      }
      (*numberOfProducts)--;
      found = 1;
      break;
    }
  }

  if (!found)
  {
    printf("Product with ID %d not found.\n", deleteID);
    return productDetails;
  }

  productDetails = realloc(productDetails, (*numberOfProducts) * sizeof(ProductDetails));
  if (!productDetails && *numberOfProducts > 0)
  {
    printf("Memory reallocation failed.\n");
  }
  else
  {
    printf("Product deleted successfully.\n");
  }

  return productDetails;
}

void processInventory(ProductDetails **productDetails, int *numberOfProducts)
{
  int choice;
  do
  {
    inventoryMenu();
    printf("Enter your choice: ");
    scanf("%d", &choice);

    switch (choice)
    {
    case 1:
      *productDetails = addNewProduct(*productDetails, numberOfProducts);
      break;
    case 2:
      viewAllProducts(*productDetails, *numberOfProducts);
      break;
    case 3:
      updateQuantityOfProduct(*productDetails, *numberOfProducts);
      break;
    case 4:
      searchProductByID(*productDetails, *numberOfProducts);
      break;
    case 5:
      searchProductByName(*productDetails, *numberOfProducts);
      break;
    case 6:
      searchProductByPriceRange(*productDetails, *numberOfProducts);
      break;
    case 7:
      *productDetails = deleteProductByID(*productDetails, numberOfProducts);
      break;
    case 8:
      printf("Memory released successfully. Exiting program....\n");
      break;
    default:
      printf("Invalid choice. Try again.\n");
    }
  } while (choice != 8);
}
