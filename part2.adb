with Ada.Text_IO;
	use Ada.Text_IO;

with Ada.Calendar;
	use Ada.Calendar;

with Ada.Numerics.Float_Random;
	use Ada.Numerics.Float_Random;

procedure part2 is
	start : constant Time := Clock;
	runF3 : Boolean := False;
	F3StartTime : Time;
	F3EndTime : Time;
	loopStartTime : Time;
	iterations : Integer := 0;
	delayFlag : Boolean := False;
	delayTime : Duration;

	-- Task Watchdog
	task Watchdog is
		entry Init;
		entry F3Done;
	end Watchdog;

	task body Watchdog is
	begin 
		loop
			accept Init;

			select
				accept F3Done;
			or
				delay until Clock + 0.5;
				Put_Line("F3 missed deadline, RESYNC");
				accept F3Done;
				-- starta återsynk

			end select;

		end loop;
	end Watchdog;

	-- Task F1
	procedure F1 is
		now : constant Time := Clock;
	begin 
		Put("F1 started at: ");
		Put_Line(Duration'Image(now-start));
	end F1;

	-- Task F2
	procedure F2 is
		now : constant Time := Clock;
	begin 
		Put("F2 started at: ");
		Put_Line(Duration'Image(now-start));
	end F2;

	-- Task F3
	procedure F3 is
		now : constant Time := Clock;
		
		g : generator ;
		num : float range 0.0 .. 1.0;
	begin 
		Reset(g);
		num := Random(g);

		if (num > 0.5) then
			delay until Clock + 0.6;
			--Put("F3 random delayed");
			--Put_Line(Float'Image(num));

		end if;

		Put("F3 started at: ");
		Put_Line(Duration'Image(now-start));
		Watchdog.F3Done;
	end F3;
	
-- Main block
begin
	loop
		loopStartTime := Clock;
		
		F1;
		F2;

		if (runF3) then
			delay until loopStartTime + 0.5;

			Watchdog.Init;
			F3StartTime := Clock;
			F3;
			F3EndTime := Clock;
			iterations := iterations + 1;
			runF3 := False;
		else
			runF3 := True;
		end if;

		if (F3EndTime - F3StartTime > 0.5) then
			delayTime := Clock - Clock;
			loop
				if (delayTime < F3EndTime - loopStartTime) then
					delayTime := delayTime + 1.0;
				else 
					exit;
				end if;

			end loop;
			--Put_Line(Duration'Image(delayTime));
			--Put_Line(Duration'Image(F3EndTime - F3StartTime));
			delay until loopStartTime + delayTime;
		else 
			delay until loopStartTime + 1.0;
		end if;


	end loop;
end part2;