-- Lab 1: Anders Hassis,  Ludvig Norinder & Oskar Wirén

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
		entry Finish;
		entry Put(item : in Integer);
		entry Get(item : out Integer);
	end BufferTask;

	-- PutterTask
	task PutterTask is
		entry Init;
		entry Finish;
	end PutterTask;

	task body BufferTask is
		buffer : Array(0..10) of Integer;
		firstItem : Integer := 0;
		lastItem : Integer := 0;
		itemCount : Integer := 0;
		finished : Boolean := false;
		
		-- Check if buffer is full
		function isFull return Boolean is
		begin
			return (lastItem + 1) mod 10 = firstItem;
		end;

		-- Check if buffer is empty
		function isEmpty return Boolean is
		begin
			return firstItem = lastItem;
		end;

		-- Return amount of items in buffer
		function getItemCount return Integer is
		begin
			return itemCount;
		end;

		-- Enqueue element into buffer
		procedure enqueue(value : in Integer) is
		begin
			if not isFull then
				buffer(lastItem) := value;
				lastItem := (lastItem + 1) mod 10;
				itemCount := itemCount + 1;
			end if;
		end;

		-- Dequeue element from buffer
		procedure dequeue(value : out Integer) is
		begin
			if not isEmpty then
				value := buffer(firstItem);
				firstItem := (firstItem + 1) mod 10;
				itemCount := itemCount - 1;
			end if;
		end;

	begin
		accept Init;

		-- Loop until GetterTask indicates by setting finished = true
		while not finished loop
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
			or
				accept Finish do
					finished := true;
					PutterTask.Finish;
				end Finish;
			end select;
		end loop;

		Put_Line("Buffertask is quitting");
	end BufferTask;

	task body PutterTask is
		g : generator;
		num : Integer;
		finished : Boolean := false;
	begin 
		Reset(g);
		accept Init;

		-- Loop until BufferTask indicates by setting finished = true
		while not finished loop
			select
				accept Finish do
					finished := true;
				end Finish;
			or
				-- Delay between 0 and 1 second
				delay Duration(Random(g) * 1.0);
				num := Integer(Random(g) * 25.0);
				BufferTask.Put(num);
				Put_Line("Put:" & Integer'Image(num));
			end select;
		end loop;
		Put_Line("PutterTask is quitting");
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

		-- Run until sum is > 100
		while sum < 100 loop
			delay Duration(Random(g) * 2.0);
			BufferTask.Get(value);
			sum := sum + value;
			Put_Line("Get:" & Integer'Image(value) & ", Sum:" & Integer'Image(sum));
		end loop;

		-- Notify the buffer that we are done
		BufferTask.Finish;
		Put_Line("Gettertask is quitting");
	end GetterTask;

begin
	BufferTask.Init;
	PutterTask.Init;
	GetterTask.Init;
end part3;
