with Ada.Text_IO;
	use Ada.Text_IO;

with Ada.Calendar;
	use Ada.Calendar;

procedure part1 is
	start : constant Time := Clock;
	runF3 : Boolean := False;
	loopStartTime : Time;

	procedure F1 is
		now : constant Time := Clock;
	begin 
		Put("F1 started at: ");
		Put_Line(Duration'Image(now-start));
	end F1;

	procedure F2 is
		now : constant Time := Clock;
	begin 
		Put("F2 started at: ");
		Put_Line(Duration'Image(now-start));
	end F2;

	procedure F3 is
		now : constant Time := Clock;
	begin 
		Put("F3 started at: ");
		Put_Line(Duration'Image(now-start));
	end F3;
begin
	loop
		loopStartTime := Clock;

		F1;
		F2;

		if (runF3) then
			delay until loopStartTime + 0.5;
			F3;
			runF3 := False;
		else
			runF3 := True;
		end if;

		delay until loopStartTime + 1.0;

	end loop;
end part1;