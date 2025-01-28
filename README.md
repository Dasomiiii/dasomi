A tailor-made static site generator CLI for my website: [https://dasomi.neocities.org](Dasomi).

# Compiling

Create a directory called build and change directory to it, then run `cmake ..`

# Program Usage

Create a directory and put all your resource files there, like images, fonts, stylesheets and post files.
Then run `./dasomi -resources your_resource_directory -output your_output_directory`

There are some example post files in resources/ that you can look at to get

It doesn't matter what subdirectories you put your files in; all file types will automatically get sorted
to their respective subdirectories in the output directory. Images will get put into images/, fonts will
be put into fonts/, etc.

Who am I kidding? No one but me's gonna fucking use this program! I just want the codebase to be comprehensible,
that's all.
