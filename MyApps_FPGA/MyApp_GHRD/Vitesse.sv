

module Vitesse(
	input logic a,
	input logic b,
	input logic c,
	input logic d,
	input logic clk,
	output logic [31:0] res);
	
	logic SpeedReset = 0;
	
	logic [15:0] counterA = 0;
	logic [15:0] counterB  = 0;
	logic [15:0] counterC = 0;
	logic [15:0] counterD = 0;
	logic [15:0] counterAbis = 0;
	logic [15:0] counterBbis = 0;
	logic [15:0] counterCbis = 0;
	logic [15:0] counterDbis = 0;
	
	logic [14:0] ResLeft = 0;
	logic [14:0] ResRight= 0;
	logic 		  SensLeft= 0;
	logic 		  SensRight=0;
	logic 		  reset=0;
	
	logic [31:0] Time=0;
	
	always_ff @(posedge clk, posedge reset)
		begin
			if(reset)
				begin
					ResLeft <= 0;
					ResRight<= 0;
				end
			else
				begin
					case(Time)
						32'd5000000:	begin		
											Time <= 0;
											SpeedReset <= 1;
											ResLeft <= counterA + counterB + counterAbis + counterBbis;
											ResRight <= counterC + counterD + counterCbis + counterDbis;
										end																			
						default:	begin
										Time <= Time + 1;
										SpeedReset <= 0;
									end
					endcase
				end
		end
					
	always @(posedge a or posedge SpeedReset)
		if(SpeedReset)
			counterA<= 0;
		else begin
			if(b==0)	begin
				SensLeft <= 1;
				counterA <= counterA + 1;
			end
			else begin
				SensLeft <= 0;
				counterA <= counterA + 1;
			end
		end
	
	always @(posedge b or posedge SpeedReset)
		if(SpeedReset)
			counterB<= 0;
		else
			counterB <= counterB + 1;
		
	always @(posedge c or posedge SpeedReset)
		if(SpeedReset)
			counterC<= 0;
		else begin
			if(d==0)	begin
				SensRight <= 1;
				counterC <= counterC + 1;
			end
			else begin
				SensRight <= 0;
				counterC <= counterC + 1;
			end
		end
		
	always @(posedge d or posedge SpeedReset)
		if(SpeedReset)
			counterD<= 0;
		else
		counterD <= counterD + 1;
	always @(negedge a or posedge SpeedReset)
		if(SpeedReset)
			counterAbis <= 0;
		else
			counterAbis <= counterAbis + 1;
	
	always @(negedge b or posedge SpeedReset)
		if(SpeedReset)
			counterBbis<= 0;
		else
			counterBbis <= counterBbis + 1;
		
	always @(negedge c or posedge SpeedReset)
		if(SpeedReset)
			counterCbis<= 0;
		else
			counterCbis <= counterCbis + 1;
		
	always @(negedge d or posedge SpeedReset)
		if(SpeedReset)
			counterDbis<= 0;
		else
		counterDbis <= counterDbis + 1;
	
	assign res = {SensLeft, ResLeft, SensRight, ResRight};

endmodule