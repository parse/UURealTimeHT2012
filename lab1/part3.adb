with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Calendar;
use Ada.Calendar;

with Ada.Numerics.Float_Random;
use Ada.Numerics.Float_Random;

procedure part3 is
	-- BufferTask
	task BufferTask is
		entry Init;
		entry Put(item : in Integer);
		entry Get(item : out Integer);
	end BufferTask;

	task body BufferTask is
		buffer : Array(0..10) of Integer;
		firstItem : Integer := 0;
		lastItem : Integer := 0;
		itemCount : Integer := 0;
		
		function isFull return Boolean is
		begin
			return (lastItem + 1) mod 10 = firstItem;
		end;

		function isEmpty return Boolean is
		begin
			return firstItem = lastItem;
		end;

		function getItemCount return Integer is
		begin
			return itemCount;
		end;

		procedure enqueue(value : in Integer) is
		begin
			if not isFull then
				buffer(lastItem) := value;
				lastItem := (lastItem + 1) mod 10;
				itemCount := itemCount + 1;
			end if;
			-- Put_Line("Items in queue:" & Integer'Image(getItemCount));
		end;

		procedure dequeue(value : out Integer) is
		begin
			if not isEmpty then
				value := buffer(firstItem);
				firstItem := (firstItem + 1) mod 10;
				itemCount := itemCount - 1;
			end if;
			-- Put_Line("Items in queue:" & Integer'Image(getItemCount));
		end;

	begin
		accept Init;
		loop
			select
				when not isFull =>
					accept Put(item : in Integer) do
						enqueue(item);
					end Put;
			or
				when not isEmpty =>
					accept Get(item : out Integer) do
						dequeue(item);
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
		Reset(g);
		accept Init;
		loop
			delay Duration(Random(g) * 1.0);
			num := Integer(Random(g) * 25.0);
			BufferTask.Put(num);
			Put_Line("Put:" & Integer'Image(num));
		end loop;
	end PutterTask;

	-- GetterTask
	task GetterTask is
		entry Init;
	end GetterTask;

	task body GetterTask is
		g : Generator;
		value : Integer := 0;
		sum : Integer := 0;
	begin
		Reset(g);
		accept Init;
		loop
			delay Duration(Random(g) * 2.0);
			BufferTask.Get(value);
			sum := sum + value;
			Put_Line("Get:" & Integer'Image(value) & " sum:" & Integer'Image(sum));
			
			if sum > 100 then
				Put_Line("Jesus-titty-fucking-christ I've reached ONE_HUNDRED!");
			end if;
		end loop;
	end GetterTask;

begin
	BufferTask.Init;
	PutterTask.Init;
	GetterTask.Init;
end part3;
