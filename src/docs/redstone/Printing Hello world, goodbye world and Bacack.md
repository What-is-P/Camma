first, let's start with /title.

```redstone
/title
```

then we add the option nongui-text

```redstone
/title nogui-text
```

and finally let's add the desired textes.

```redstone
/title nogui-text Hello world
```

```redstone
/title nogui-text Goodbye world
```

```redstone
/title nogui-text Bacack
```

and let's mix them together, let's also add a 6 7 version sign on them.

```redstone
/title nogui-text Hello world, goodbye world and bacack (6 7) the fucking 6 7!
```

and if you want to add HTML color formatting to the text, same way as how geometry dash uses text colors, you have to do invalid html syntax like this (no mixing or extra properties):

```redstone
/title nogui-text Hello World color:(part*"Hello*:1"+=<red><i>i = true</red></i></red><//red> part*sqrt::oversimplify.simplify=::depend.syntax=true con::sqrt::con:"World*:2"+=<purple-light>pt=lilás i=true</purple-light><//purple-light>:; color::sqrt::color-print-no;extra.no.extra.sqrt.with.::.=true.noequal.true.values.all/ return print; nil = true in all; return)

and if you want to print nothing, it's something like this:

```redstone
/title noop true
```

and if you want to print code instead of text, go like this:

```redstone
/title code true ("")
```

and thanks for reading, bye.