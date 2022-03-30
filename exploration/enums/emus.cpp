#include <iostream>

int main()
{
    enum test : unsigned short { ace = 1, two, three = 4, four, five, six, seven, eight, nine, ten, jack, queen, king, none };
    test testval = test{ ace };
    test testval2 = test{ two };
    test testval3 = test{ three };
    test testval4 = test{ four };

    std::cout << static_cast<int>(testval) << std::endl;
    std::cout << static_cast<int>(testval2) << std::endl;
    std::cout << static_cast<int>(testval3) << std::endl;
    std::cout << static_cast<int>(testval4) << std::endl;
    return 0;
}