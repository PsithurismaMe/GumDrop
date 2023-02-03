If under some miracle more than one person contributes to this project, here are some guidelines.

1. Code
   1. Use this style of curly braces
   ```
   if (1)
   {
		std::cout << "Hello World" << std::endl;
   }
   ```
   2. Use pass-by-reference instead of pointers as much as possible, however don't overwrite code which does.
   + If you don't know how to pass a reference to a std::thread constructor,
	```
	void myFunction(int & something)
	{
		std::cout << something << '\n';
	}

	int main()
	{
		int someVar {1};
		std::thread myThread(myFunction, std::ref(someVar));
		myThread.join();
		return 0;
	}
	```
	3. If a if-else statement only needs to perform two functions, use the [ternary operator](https://www.w3schools.com/cpp/cpp_conditions_shorthand.asp) if possible.
	4. Use C-Style pointers and casting. If you can give a reason other than memory safety to not, then make an issue.
	5. MAKE ALL CLASS ATTRIBUTES AND METHODS PUBLIC!
	6. Avoid tying game physics to the framerate.
	7. If a framerate cap is implemented, make it toggleable within the game.
	8. Never use `using namespace ...`
	9. Don't include custom levels when pushing commits.
2. Game Assets
   1. GIMP may complain about [assets/tilesheet.png](assets/tilesheet.png) having a custom color profile. DO NOT CHANGE IT.
   2. If contributing creative work (including but not limited to images, textures, artwork, music, etc.), you agree to license it under the [The GNU General Public License v3.0](LICENSE).