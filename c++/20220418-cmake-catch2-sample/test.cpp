#include <catch2/catch_all.hpp>

unsigned int Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    REQUIRE( Factorial(1) == 1 );
    REQUIRE( Factorial(2) == 2 );
    REQUIRE( Factorial(3) == 6 );
    REQUIRE( Factorial(10) == 3628800 );
}

std::uint64_t Fibonacci(std::uint64_t number) {
    return number < 2 ? 1 : Fibonacci(number - 1) + Fibonacci(number - 2);
}

TEST_CASE("Fibonacci") {
    CHECK(Fibonacci(0) == 1);
    // some more asserts..
    CHECK(Fibonacci(5) == 8);
    // some more asserts..

    // now let's benchmark:
    BENCHMARK("Fibonacci 20") {
        return Fibonacci(20);
    };

    BENCHMARK("Fibonacci 25") {
        return Fibonacci(25);
    };

    BENCHMARK("Fibonacci 30") {
        return Fibonacci(30);
    };

    BENCHMARK("Fibonacci 35") {
        return Fibonacci(35);
    };
}

// TEST_CASE("Fibonacci") {
//     CHECK(Fibonacci(0) == 1);
//     // some more asserts..
//     CHECK(Fibonacci(5) == 8);
//     // some more asserts..

//     // now let's benchmark:
//     BENCHMARK("Fibonacci 20") {
//         return Fibonacci(20);
//     };

//     BENCHMARK("Fibonacci 25") {
//         return Fibonacci(25);
//     };

//     BENCHMARK("Fibonacci 30") {
//         return Fibonacci(30);
//     };

//     BENCHMARK("Fibonacci 35") {
//         return Fibonacci(35);
//     };

    
//     BENCHMARK("simple"){ return Fibonacci(37); };

//     BENCHMARK_ADVANCED("advanced")(Catch::Benchmark::Chronometer meter) {
//         set_up();
//         meter.measure([] { return Fibonacci(37); });
//     };
// }
