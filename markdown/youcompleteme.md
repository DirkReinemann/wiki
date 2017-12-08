<!-- toc -->
  * [youcompleteme](#youcompleteme)
    * [compile](#compile)
    * [display error messages](#display-error-messages)
<!-- toc -->


# youcompleteme

[Website](https://valloric.github.io/YouCompleteMe/)

## compile

```bash
cmake -G "Unix Makefiles" -DUSE_SYSTEM_LIBCLANG=ON . ~/.vim/bundle/YouCompleteMe/third_party/ycmd/cpp
cmake --build . --target ycm_core --config Release
```

## display error messages

```vim
:messages
:YcmDebugInfo
```
