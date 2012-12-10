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

TEST_F(ECGControllerTest, ReadFileValidWithExtensionFullPathTest)
{
  EXPECT_EQ(true, controller.readFile("C:/Users/Tomko/Documents/GitHub/ECG-analyzer/100.hea"));
}

TEST_F(ECGControllerTest, ReadAllFiles)
{
  EXPECT_EQ(true, controller.readFile("ecgSignals/100"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/101"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/102"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/103"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/104"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/105"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/106"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/107"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/108"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/109"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/111"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/112"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/113"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/114"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/115"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/116"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/117"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/118"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/119"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/121"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/122"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/123"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/124"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/200"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/201"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/202"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/203"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/205"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/207"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/208"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/209"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/210"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/212"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/213"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/214"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/215"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/217"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/219"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/220"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/221"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/222"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/223"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/228"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/230"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/231"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/232"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/233"));
  EXPECT_EQ(true, controller.readFile("ecgSignals/234"));
}

TEST_F(ECGControllerTest, RUNBASELINE)
{
  ASSERT_EQ(true, controller.readFile("ecgSignals/100"));
  EXPECT_NO_THROW(controller.runECGBaseline());
}

TEST_F(ECGControllerTest, RUNRPEAKS)
{
  ASSERT_EQ(true, controller.readFile("ecgSignals/100"));
  EXPECT_NO_THROW(controller.runRPeaks());
}

TEST_F(ECGControllerTest, RUNWAVES)
{
  ASSERT_EQ(true, controller.readFile("ecgSignals/100"));
  EXPECT_NO_THROW(controller.runWaves());
}

int main(int argc, char** argv) 
{ 
	char current_path[256];
    getcwd(current_path, 255);
	printf("Current working directory:");
    printf("\n%s\n", current_path);
	printf("\nStarting tests...\n\n");

    testing::InitGoogleTest(&argc, argv); 
    RUN_ALL_TESTS();
	std::getchar();
}
