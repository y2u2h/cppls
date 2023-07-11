/*
 * MIT License
 *
 * Copyright (c) 2023 yyuu
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>

std::string printbits(const uint64_t code, const uint32_t size) {
    auto pos = size;
    std::string str;
    while (pos--) str += ((code >> pos) & 1u) ? '1' : '0';
    return str;
}

void golomb(const unsigned int data, const unsigned int m, uint64_t &code, uint32_t &size) {
    auto q = data / m;
    auto r = data % m;
    auto b = static_cast<unsigned int>(std::log2(m));

    uint32_t rsize = 0;
    uint64_t rcode = 0;
    if (r < ((1 << (b + 1)) - m)) {
        rcode = r;
        rsize = b;
    } else {
        rcode = r + ((1 << (b + 1)) - m);
        rsize = b + 1;
    }

    uint32_t qsize = q + 1;
    uint64_t qcode = 1 << rsize;

    code = qcode | rcode;
    size = qsize + rsize;
}

void exp_golomb(const unsigned int data, const unsigned int k, uint64_t &code, uint32_t &size) {
    uint64_t mask = 1;

    code = data + (1 << k);
    size = 0;
    while (mask <= code) {
        mask <<= 1;
        size += 2;
    }
    size -= (1 + k);
}

static void print_usage() {
    std::cout << "usage: golomb <mode> [options]" << std::endl;
    std::cout << "mode:" << std::endl;
    std::cout << "    -sg,--signed-golomb" << std::endl;
    std::cout << "    -ug,--unsigned-golomb" << std::endl;
    std::cout << "    -seg,--signed-exp-golomb" << std::endl;
    std::cout << "    -ueg,--unsigned-exp-golomb" << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "    -k,--k <val> (default: 0)" << std::endl;
    std::cout << "        paramter K used by exponential golomb mode" << std::endl;
    std::cout << "    -m,--m <val> (default: 1)" << std::endl;
    std::cout << "        paramter M used by signed-golomb and unsigned-golomb mode" << std::endl;
    std::cout << "    -r,--range <start_val> <end_val> (default: -16, 16)" << std::endl;
    std::cout << "        range of displayed values" << std::endl;
    std::cout << "    --normal-mapping (default)" << std::endl;
    std::cout << "        original values : -4, -3, -2, -1,  0,  1,  2,  3, ..." << std::endl;
    std::cout << "        mapped values   :  8,  6,  4,  2,  0,  1,  3,  5, ..." << std::endl;
    std::cout << "    --jls-regular-mapping (optional)" << std::endl;
    std::cout << "        original values : -4, -3, -2, -1,  0,  1,  2,  3, ..." << std::endl;
    std::cout << "        mapped values   :  7,  5,  3,  1,  0,  2,  4,  6, ..." << std::endl;
    std::cout << "    --jls-special-mapping (optional)" << std::endl;
    std::cout << "        original values : -4, -3, -2, -1,  0,  1,  2,  3, ..." << std::endl;
    std::cout << "        mapped values   :  6,  4,  2,  0,  1,  3,  5,  7, ..." << std::endl;
}

int main(int argc, char *argv[]) {
    const auto argn = argc - 1;
    auto mode = 0;
    auto mapping = 0;
    auto k = 0;
    auto m = 1;
    auto sval = -16;
    auto eval = 16;

    for (auto i = 1; i < argc;) {
        const std::string str{argv[i]};

        if (str == "-sg" || str == "--signed-golomb") {
            mode = 1;
            i += 1;
        } else if (str == "-ug" || str == "--unsigned-golomb") {
            mode = 2;
            sval = 0;
            i += 1;
        } else if (str == "-seg" || str == "--signed-exp-golomb") {
            mode = 3;
            i += 1;
        } else if (str == "-ueg" || str == "--unsigned-exp-golomb") {
            mode = 4;
            sval = 0;
            i += 1;
        } else if (str == "-k" || str == "--k") {
            if (i + 1 > argn) {
                print_usage();
                return EXIT_FAILURE;
            }
            k = std::stoi(argv[i + 1], nullptr, 10);
            i += 2;
        } else if (str == "-m" || str == "--m") {
            if (i + 1 > argn) {
                print_usage();
                return EXIT_FAILURE;
            }
            m = std::stoi(argv[i + 1], nullptr, 10);
            i += 2;
        } else if (str == "-r" || str == "--range") {
            if (i + 2 > argn) {
                print_usage();
                return EXIT_FAILURE;
            }
            sval = std::stoi(argv[i + 1], nullptr, 10);
            eval = std::stoi(argv[i + 2], nullptr, 10);
            i += 3;
        } else if (str == "--normal-mapping") {
            mapping = 0;
            i += 1;
        } else if (str == "--jls-regular-mapping") {
            mapping = 1;
            i += 1;
        } else if (str == "--jls-special-mapping") {
            mapping = 2;
            i += 1;
        } else {
            print_usage();
            return EXIT_FAILURE;
        }
    }

    if (argn < 1) {
        print_usage();
        return EXIT_FAILURE;
    }

    if ((mode == 2) || (mode == 4) || (mode == 6)) {
        if ((sval < 0) | (eval < 0)) {
            std::cout << "range of values must be equal or more than zero." << std::endl;
            std::cout << std::endl;
            print_usage();
            return EXIT_FAILURE;
        }
    }

    auto map_normal  = [](const int &val) { return (val <= 0) ? -2 * val     :  2 *  val - 1 ; };
    auto map_regular = [](const int &val) { return (val >= 0) ?  2 * val     : -2 *  val - 1 ; };
    auto map_special = [](const int &val) { return (val >= 0) ?  2 * val + 1 : -2 * (val + 1); };

    for (auto i = sval; i <= eval; ++i) {
        uint64_t code = 0;
        uint32_t size = 0;
        auto mval = 0;

        switch (mode) {
            case 1:  // signed-golomb
                mval = (mapping == 0) ? map_normal(i) :
                       (mapping == 1) ? map_regular(i):
                                        map_special(i);
                golomb(mval, m, code, size);
                break;
            case 2:  // unsigned-golomb
                mval = i;
                golomb(mval, m, code, size);
                break;
            case 3:  // signed-exp-golomb
                mval = (mapping == 0) ? map_normal(i) :
                       (mapping == 1) ? map_regular(i):
                                        map_special(i);
                exp_golomb(mval, k, code, size);
                break;
            case 4:  // unsigned-exp-golomb
                mval = i;
                exp_golomb(mval, k, code, size);
                break;
            default:
                print_usage();
                return EXIT_FAILURE;
        }

        std::cout << std::setw(4) << std::right <<    i << " "
                  << std::setw(4) << std::right << mval << " "
                  << std::setw(4) << std::right << size << " "
                  << printbits(code, size) << std::endl;
    }

    return 0;
}
