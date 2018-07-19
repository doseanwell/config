#批量删除制定类型的文件
find . -name "*~" -type f -print -exec rm -rf {} \;
