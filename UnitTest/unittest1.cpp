#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "fixedp.h"

namespace UnitTest
{		

	inline bool almost_equal(float lhs, float rhs, float tolerance)
	{
		return fabs(lhs-rhs) <= tolerance * std::max(1.0f, std::max(lhs, rhs));
	}
	inline bool almost_equal(float lhs, float rhs, float abs_tolerance, float rel_tolerance)
	{
		float diff = fabs(lhs-rhs);
		return (diff <= abs_tolerance) ||  (diff <= rel_tolerance * std::max(lhs, rhs));
	}

	TEST_CLASS(UnitTest1)
	{
	public:
		
		TEST_METHOD(test_fixed_binary)
		{
			using namespace wyc;
			typedef xfixed_binary<8,short> fp_t;
			// fixed point 8.8 gives about 2 decimal digits precision
			float abs_tolerance = 0.002f, rel_tolerance = 0.0001f;
			fp_t f1 = 3.14, f2=3;
			Assert::IsTrue(almost_equal(float(f1),3.14f,abs_tolerance,rel_tolerance));
			Assert::AreEqual(int(f1),3);
			Assert::AreEqual(int(f2),3);
			f1 = -6.28;
			f2 = -6;
			Assert::IsTrue(almost_equal(float(f1),-6.28f,abs_tolerance,rel_tolerance));
			Assert::AreEqual(int(f1),-6);
			Assert::AreEqual(int(f2),-6);
			fp_t f3 = f1, f4;
			f4 = f2;
			Assert::IsTrue(f1==f3);
			Assert::IsTrue(f2==f4);
			f1 = 3.14;
			f2 = 6.28;
			Assert::IsTrue(f1!=f2);
			Assert::IsTrue(f1<f2);
			Assert::IsTrue(f2>f1);
			f1 = -3.14;
			f2 = -6.28;
			Assert::IsTrue(f1!=f2);
			Assert::IsTrue(f1>f2);
			Assert::IsTrue(f2<f1);
			float v1 = float(f1);
			float v2 = float(f2);
			Assert::IsTrue(almost_equal(float(f1+f2),v1+v2,abs_tolerance,rel_tolerance));
			Assert::IsTrue(almost_equal(float(f1-f2),v1-v2,abs_tolerance,rel_tolerance));
			Assert::IsTrue(almost_equal(float(f1*f2),v1*v2,abs_tolerance,rel_tolerance));
			Assert::IsTrue(almost_equal(float(f1/f2),v1/v2,abs_tolerance,rel_tolerance));
		}

	};
}