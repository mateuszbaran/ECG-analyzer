#include "gtest/gtest.h"

#include "ECGController.h"

class ECGControllerTest : public ::testing::Test
{
protected:

  ECGController controller;
};

TEST_F(ECGControllerTest, ReadFileInvalidTest)
{
  EXPECT_EQ(false, controller.readFile("no_file_QWE"));
}

TEST_F(ECGControllerTest, ReadFileValidTest)
{
  EXPECT_EQ(true, controller.readFile("100"));
}

TEST_F(ECGControllerTest, ReadFileValidWithExtensionTest)
{
  EXPECT_EQ(true, controller.readFile("100.hea"));
}

TEST_F(ECGControllerTest, ReadAllFiles)
{
  EXPECT_EQ(true, controller.readFile("100"));
  EXPECT_EQ(true, controller.readFile("101"));
  EXPECT_EQ(true, controller.readFile("102"));
  EXPECT_EQ(true, controller.readFile("103"));
  EXPECT_EQ(true, controller.readFile("104"));
  EXPECT_EQ(true, controller.readFile("105"));
  EXPECT_EQ(true, controller.readFile("106"));
  EXPECT_EQ(true, controller.readFile("107"));
  EXPECT_EQ(true, controller.readFile("108"));
  EXPECT_EQ(true, controller.readFile("109"));
  EXPECT_EQ(true, controller.readFile("111"));
  EXPECT_EQ(true, controller.readFile("112"));
  EXPECT_EQ(true, controller.readFile("113"));
  EXPECT_EQ(true, controller.readFile("114"));
  EXPECT_EQ(true, controller.readFile("115"));
  EXPECT_EQ(true, controller.readFile("116"));
  EXPECT_EQ(true, controller.readFile("117"));
  EXPECT_EQ(true, controller.readFile("118"));
  EXPECT_EQ(true, controller.readFile("119"));
  EXPECT_EQ(true, controller.readFile("121"));
  EXPECT_EQ(true, controller.readFile("122"));
  EXPECT_EQ(true, controller.readFile("123"));
  EXPECT_EQ(true, controller.readFile("124"));
  EXPECT_EQ(true, controller.readFile("200"));
  EXPECT_EQ(true, controller.readFile("201"));
  EXPECT_EQ(true, controller.readFile("202"));
  EXPECT_EQ(true, controller.readFile("203"));
  EXPECT_EQ(true, controller.readFile("205"));
  EXPECT_EQ(true, controller.readFile("207"));
  EXPECT_EQ(true, controller.readFile("208"));
  EXPECT_EQ(true, controller.readFile("209"));
  EXPECT_EQ(true, controller.readFile("210"));
  EXPECT_EQ(true, controller.readFile("212"));
  EXPECT_EQ(true, controller.readFile("213"));
  EXPECT_EQ(true, controller.readFile("214"));
  EXPECT_EQ(true, controller.readFile("215"));
  EXPECT_EQ(true, controller.readFile("217"));
  EXPECT_EQ(true, controller.readFile("219"));
  EXPECT_EQ(true, controller.readFile("220"));
  EXPECT_EQ(true, controller.readFile("221"));
  EXPECT_EQ(true, controller.readFile("222"));
  EXPECT_EQ(true, controller.readFile("223"));
  EXPECT_EQ(true, controller.readFile("228"));
  EXPECT_EQ(true, controller.readFile("230"));
  EXPECT_EQ(true, controller.readFile("231"));
  EXPECT_EQ(true, controller.readFile("232"));
  EXPECT_EQ(true, controller.readFile("233"));
  EXPECT_EQ(true, controller.readFile("234"));
}

int main(int argc, char** argv) 
{ 
    testing::InitGoogleTest(&argc, argv); 
    RUN_ALL_TESTS(); 
}
