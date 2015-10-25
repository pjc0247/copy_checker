copy_finder
====

__copy_finder__ can help you to find unexpected copy operation on your C++ code. If an unexpected copy occurred, it will print the callstack before entering the copy constructor.

Usage
----

Options
----
You can specify behavior when `copy_checker` detects unallowed copy operation.
* `CC_BREAK`
  * triggers a breakpoint and the program will be stopped immediately.
* `CC_LOG`
  * prints callstack log to `stdout` and the program will keep running. 
* `CC_PROFILE`
  * prints all copy points at program ends.
* `CC_IGNORE`
  * does nothing.

Example
----
```c++
class Hello : copy_checker {
};

Hello foo() {
	Hello dd;

	return dd;
}
Hello foo(Hello &f) {
	return f;
}

int main() {
	Hello a = foo(); // passsssssss

	Hello b = foo(a); // omg

  return 0;
}
```
![img](img/print_log.png)

Remarks
----
* This program requires Visual Studio 2015 or higher. (Windows only)
