module adder (
    input  [3:0] a,
    input  [3:0] b,
    output [4:0] sum
);
    assign sum = a + b;
endmodule

module top (
    input  [3:0] x,
    input  [3:0] y,
    output [4:0] result
);
    // Instantiate the adder module
    adder my_adder (
        .a(x),       // connect x to input a of adder
        .b(y),       // connect y to input b of adder
        .sum(result) // connect result to output sum of adder
    );
endmodule
