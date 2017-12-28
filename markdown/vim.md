<!-- toc -->
  * [vim](#vim)
    * [display error messages](#display-error-messages)
    * [youcompleteme](#youcompleteme)
    * [clang_complete](#clang_complete)
<!-- toc -->


# vim

## display error messages

```vim
:messages
```

## youcompleteme

[Website](https://valloric.github.io/YouCompleteMe/)

```bash
mkdir ~/.vim/bundle/youcompleteme/ycm_build
cd ~/.vim/bundle/youcompleteme/ycm_build
cmake -G "Unix Makefiles" -DUSE_SYSTEM_LIBCLANG=ON . ~/.vim/bundle/youcompleteme/third_party/ycmd/cpp
cmake --build . --target ycm_core --config Release

### go
cd ~/.vim/bundle/youcompleteme/third_party/ycmd/third_party/gocode
go build

### typescript
sudo npm install -g typescript

### javascript
cd ~/.vim/bundle/youcompleteme/third_party/ycmd/third_party/tern_runtime
npm install --production

### rust
cd ~/.vim/bundle/youcompleteme/third_party/ycmd/third_party/racerd
cargo build --release
```

```vim
let g:ycm_global_ycm_extra_conf = '~/.vim/.ycm_extra_conf.py'
let g:ycm_collect_identifiers_from_tags_files = 1
let g:ycm_server_python_interpreter = "/usr/bin/python2"
```

```vim
:YcmDebugInfo
```

## clang_complete

[Website](https://github.com/rip-rip/clang_complete)

```bash
vi ~/.vim/bundle/clang_complete/bin/cc_args.py

vim> #!/usr/bin/env python2
```

## syntastic

### markdown

[markdownlint]: https://github.com/markdownlint/markdownlint

```bash
gem install mdl
```
