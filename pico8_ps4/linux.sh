intdir=linux_build

mkdir $intdir

set -e # Exit script when any error happens

for file in *.cpp; do
  filename="${file%.*}"
  if [ "$file" -nt "$intdir/$filename.o" ]
  then
    echo "clang++ -c -o \"$intdir/$filename.o\" $file"
    clang++ -std=c++11 -I"../lua" -I"../libfixmath" -I"../lib/include" -I"/usr/include/SDL2" -D__LINUX__ -c -o "$intdir/$filename.o" $file
  fi
done

if [ "$1" != "skiprun" ]; then
  echo "clang++ $intdir/*.o ../lua/$intdir/*.o ../libfixmath/$intdir/*.o -lSDL2 -lcurl -lpthread -o \"$intdir/ps4-p8\""
  clang++ $intdir/*.o ../lua/$intdir/*.o ../libfixmath/$intdir/*.o -lSDL2 -lcurl -lpthread -o "$intdir/ps4-p8"
  "./$intdir/ps4-p8"
fi
