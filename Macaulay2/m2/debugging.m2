--		Copyright 1993-1999 by Daniel R. Grayson

olderror := error
erase symbol error
error = args -> olderror apply(
     sequence args, x -> if class x === String then x else toString x
     )
protect symbol error

on = { CallLimit => 100000 } ==> opts -> f -> (
     n := try toString f else string f;
     depth := 0;
     totaltime := 0.;
     callCount := 0;
     limit := opts.CallLimit;
     if class f =!= Function then error("expected a function");
     x -> (
	  saveCallCount := callCount = callCount+1;
     	  << n << " (" << saveCallCount << ")";
	  if depth > 0 then << " [" << depth << "]";
	  << "  called with " << class x << " : " << x << endl;
	  if callCount > limit then error "call limit exceeded";
	  depth = depth + 1;
     	  r := timing f x;
	  timeused := r#0;
	  value := r#1;
	  depth = depth - 1;
	  if depth === 0 then totaltime = totaltime + timeused;
     	  << n << " (" << saveCallCount << ")";
	  if depth > 0 then << " [" << depth << "]";
	  << " " << timeused << " seconds";
	  if depth === 0 then << ", used " << totaltime << " seconds";
	  << "  returned " << class value << " : " << value << endl;
     	  value)
     )

assert = x -> if not x then error "assertion failed"

notImplemented = x -> error "not implemented yet"

benchmark = (s) -> (
     n := 1;
     while (
	  s1 := concatenate("timing scan(",string n,", i -> (",s,";null;null))");
	  s2 := concatenate("timing scan(",string n,", i -> (      null;null))");
	  collectGarbage();
	  value s1;
	  value s2;
	  collectGarbage();
	  value s1;
	  value s2;
	  collectGarbage();
	  t1 := first value s1;
	  t2 := first value s2;
	  t := t1 - t2;
	  t < 1 and t2 < 5)
     do (
	  n = if t > 0.01 then floor(2*n/t+.5) else 100*n;
	  );
     t/n)

-----------------------------------------------------------------------------
Descent := new Type of MutableHashTable
net Descent := x -> stack sort apply(pairs x,
     (k,v) -> (
	  if #v === 0
	  then net k
	  else net k | " : " | net v))
select1 := syms -> select(apply(syms, value), s -> instance(s, Type))
     
show1 := method(SingleArgumentDispatch => true)
show1 Sequence := show1 List := types -> (
     world := new Descent;
     install := v -> (
	  w := if v === Thing then world else install parent v;
	  if w#?v then w#v else w#v = new Descent
	  );
     scan(types, install);
     net world)
show1 Thing := X -> show1 {X}
showUserStructure = Command(() -> show1 select1 userSymbols())
showStructure = Command(types -> show1 if types === () then select1 values symbolTable() else types)

-----------------------------------------------------------------------------

userSymbols = type -> (
     if type === () then type = Thing;
     tab := symbolTable();
     v := select(values tab,
	  symb -> (
	       hash symb > hash lastSystemSymbol  -- hash codes of symbols are sequential
	       and mutable symb
	       and instance(value symb,type)
	       )
	  );
     apply(sort(apply(v, symb -> (hash symb, symb))), (h,s) -> s))

listUserSymbols = new Command from (
     type -> stack apply(userSymbols type, s ->  toString s | ": " | toString class value s)
     )

clearedSymbol := "-- cleared symbol --"

clearOutput = new Command from (() -> (
     	  scan(keys outputSymbols, s -> (
	       	    remove(outputSymbols,s);
		    s <- clearedSymbol;
	       	    erase s))))

clearAll = new Command from (() -> ( clearOutput(); scan(userSymbols(), i -> i <- i)))

typicalValues#frame = MutableList
