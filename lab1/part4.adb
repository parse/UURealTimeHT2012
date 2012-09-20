with Ada.Text_IO;
use Ada.Text_IO;

with Ada.Calendar;
use Ada.Calendar;

with Ada.Numerics.Float_Random;
use Ada.Numerics.Float_Random;


procedure part4 is

	-- Define type for buffer
	type BufferType is Array(0..10) of Integer;

	-- BufferTask Header
	protected BufferTask is
		procedure Init;
		procedure Finish;
		entry Put(item : in Integer);
		entry Get(item : out Integer);
	private
		buffer : BufferType;
		firstItem : Integer := 0;
		lastItem : Integer := 0;
		itemCount : Integer := 0;
		finished : Boolean := false;
	end BufferTask;

	-- PutterTask Header
	task PutterTask is
		entry Init;
		entry Finish;
	end PutterTask;

	-- BufferTask Body
	protected body BufferTask is
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

		-- Return amount of elements in buffer
		function getItemCount return Integer is
		begin
			return itemCount;
		end;

		-- Enqueue item into queue
		procedure enqueue(value : in Integer) is
		begin
			if not isFull then
				buffer(lastItem) := value;
				lastItem := (lastItem + 1) mod 10;
				itemCount := itemCount + 1;
			end if;
			-- Put_Line("Items in queue:" & Integer'Image(getItemCount));
		end;

		-- Dequeue item from buffer
		procedure dequeue(value : out Integer) is
		begin
			if not isEmpty then
				value := buffer(firstItem);
				firstItem := (firstItem + 1) mod 10;
				itemCount := itemCount - 1;
			end if;
			-- Put_Line("Items in queue:" & Integer'Image(getItemCount));
		end;

		procedure Init is
		begin
			Put_Line("Init BufferTask");
		end Init;

		-- Listen for finishing call and end the PutterTask
		procedure Finish is
		begin
			finished := true;
			PutterTask.Finish;
			Put_Line("BufferTask quitting");
		end Finish;

		entry Put(item : in Integer) when not isFull is
		begin
			enqueue(item);
		end Put;		

		entry Get(item : out Integer) when not isEmpty is
		begin
			dequeue(item);
		end Get;	
	end BufferTask;

	-- PutterTask Body
	task body PutterTask is
		g : generator;
		num : Integer;
		finished : Boolean := false;
	begin 
		Reset(g);
		accept Init;

		-- Run until finished, receiving call from BufferTask
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

	-- GetterTask Header
	task GetterTask is
		entry Init;
	end GetterTask;

	-- GetterTask Body
	task body GetterTask is
		g : Generator;
		value : Integer := 0;
		sum : Integer := 0;
	begin
		Reset(g);
		accept Init;
		while sum < 100 loop
			delay Duration(Random(g) * 2.0);
			BufferTask.Get(value);
			sum := sum + value;
			Put_Line("Get:" & Integer'Image(value) & ", Sum:" & Integer'Image(sum));
		end loop;

		BufferTask.Finish;
		Put_Line("Gettertask is quitting");
	end GetterTask;

begin
	BufferTask.Init;
	PutterTask.Init;
	GetterTask.Init;
end part4;
