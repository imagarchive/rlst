module top (
    input wire A,
    input wire B,
    output wire Y
);

    wire not_A;
    wire not_B;
    wire A_and_not_B;
    wire B_and_not_A;

    // Inverses
    not (not_A, A);
    not (not_B, B);

    // AND gates
    and (A_and_not_B, A, not_B);
    and (B_and_not_A, B, not_A);

    // OR gate
    or (Y, A_and_not_B, B_and_not_A);

endmodule
