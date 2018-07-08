echo "Deleting old binaries..."
rm -rf ./binaries
mkdir binaries

for shadername in "$@"
do
    vertex_shader="$shadername.vert"
    fragment_shader="$shadername.frag"

    echo "Compiling vertex shader $vertex_shader to $shadername.vert.spv." 
    glslangValidator -V $vertex_shader -o "./binaries/$shadername.vert.spv"
    echo "Compiling fragment shader $fragment_shader to $shadername.frag.spv."
    glslangValidator -V $fragment_shader -o "./binaries/$shadername.frag.spv"
done



