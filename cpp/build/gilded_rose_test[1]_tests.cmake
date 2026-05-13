add_test([=[GildedRoseTest.Foo]=]  E:/dev/GildedRose_13/cpp/build/gilded_rose_test.exe [==[--gtest_filter=GildedRoseTest.Foo]==] --gtest_also_run_disabled_tests)
set_tests_properties([=[GildedRoseTest.Foo]=]  PROPERTIES DEF_SOURCE_LINE E:/dev/GildedRose_13/cpp/test/GildedRoseTest.cpp:4 WORKING_DIRECTORY E:/dev/GildedRose_13/cpp/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set(  gilded_rose_test_TESTS GildedRoseTest.Foo)
