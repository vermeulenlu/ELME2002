
module timer_for_hps(
		input clk,
		output reg[31:0] timer);
		
		reg [31:0] counter_reset;
		reg [31:0] timer_counter = 32'd0;
		
		always_ff @(posedge clk)
		begin
			case(counter_reset)
				32'd5000 : begin
						timer_counter <= timer_counter + 1;
						counter_reset <= 0;
						end
				default: begin
						counter_reset <= counter_reset + 1;
						end
				endcase
		end
		
		assign timer = timer_counter;
endmodule

			
		