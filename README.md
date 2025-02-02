# Compiling

Create a directory called build and change directory to it, then run `cmake ..`

# Program Usage

Create a directory and put all your resource files there, like images, fonts, stylesheets and post files.
Then run `./dasomi -resources your_resource_directory/ -output your_output_directory/`. If the output
directory doesn't exist, it'll be created. If it does exist, it'll get replaced.

It doesn't matter what subdirectories you put your resource files in; all file types will automatically get sorted
to their respective subdirectories in the output directory. Images will get put into images/, fonts into fonts/, etc.

There are some example post files in resources/ that you can look at to understand the post format.
