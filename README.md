A tailor-made static site generator CLI for my website: [https://dasomi.neocities.org](Dasomi).

# Compiling

Create a directory called build and change directory to it, then run `cmake ..`

# Program Usage

Create a directory and put all your resource files there, like images, fonts, stylesheets and post files.
Then run `./dasomi -resources your_resource_directory -output your_output_directory`
It doesn't matter what subdirectories you put your files in; all file types will automatically get sorted
to their respective subdirectories in the output directory. Images will get put into images/, fonts will
be put into fonts/, etc.
