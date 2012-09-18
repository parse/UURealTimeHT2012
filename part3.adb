with Ada.Text_IO;
	use Ada.Text_IO;

with Ada.Calendar;
	use Ada.Calendar;

with Ada.Numerics.Discrete_Random;
	use Ada.Numerics.Discrete_Random;

procedure part3 is
	
	-- BufferTask
	task BufferTask is
		entry Init;
		entry Put(item : in Integer);
		entry Get(item : out Integer);
	end BufferTask;

	task body BufferTask is
		buffer : Array(1..10) of Integer;
		firstItem : Integer := -1;
		lastItem : Integer := 0;
	begin 
		accept Init;
		loop
			select	
				accept Put(item : in Integer) do
					if (((lastItem + 1) mod 10) /= firstItem) then
						lastItem := lastItem + 1;
						if (firstItem = -1) then
							firstItem := lastItem;
						end if;

						buffer(lastItem) := item;
						Put_Line("Buffer Put");
					end if;
				end Put;
			or
				accept Get(item : out Integer) do
					item := buffer(firstItem);
					Put_Line("Buffer Get");
				end Get;
			end select;
			

		end loop;
	end BufferTask;

	-- PutterTask
	task PutterTask is
		entry Init;
	end PutterTask;

	task body PutterTask is
		g : generator;
		num : Integer range 0 .. 25;
	begin 
		accept Init;
		loop
			Reset(g);
			num := Random(g);
			BufferTask.Put(num);
			Put("Put");
			Put_Line(Integer'Image(num));
		end loop;
		
	end PutterTask;

		-- GetterTask
	task GetterTask is
		entry Init;
	end GetterTask;

	task body GetterTask is
		value : Integer := 0;
	begin 
		Put_Line("Get before init");
		accept Init;
		loop
			BufferTask.Get(value);
			Put("Get");
			Put_Line(Integer'Image(value));	
		end loop;
		

	end GetterTask;

begin
	BufferTask.Init;
	PutterTask.Init;
	GetterTask.Init;
end part3;