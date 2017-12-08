<!-- toc -->
  * [code snippets](#code-snippets)
    * [bash](#bash)
      * [hash string](#hash-string)
    * [scala](#scala)
      * [hash string](#hash-string)
      * [user input menu](#user-input-menu)
    * [c](#c)
      * [measure duration in milliseconds](#measure-duration-in-milliseconds)
      * [copy file](#copy-file)
    * [docker](#docker)
    * [git](#git)
      * [add repository to github](#add-repository-to-github)
      * [delete remote branch](#delete-remote-branch)
<!-- toc -->


# code snippets

## bash

### hash string

```bash
echo -n "hello" | sha512sum
```
## scala

### hash string

```scala
val md = java.security.MessageDigest.getInstance("SHA-512")
md.digest("hello".getBytes("UTF-8")).map("%02x".format(_)).mkString
```

### user input menu

```scala
def menu(): Unit = {
    println("\nMenu:\n")
    printf("%5d. %-30s\n", 1, "install tool")
    printf("%5d. %-30s\n", 2, "uninstall tool")
    printf("%5d. %-30s\n", 3, "tool versions")
    printf("%5d. %-30s\n", 4, "change tool verions")
    printf("%5d. %-30s\n\n", 5, "list tools")
    printf("%5d. %-30s\n\n", 0, "exit")
    printf("What do you want to do? ")
  }

  def list(): Unit = {
    println("\nTools:\n")
    for ((tool, index) <- Configuration.configuration.tools.zip(Stream from 1)) {
      printf("%5d. %-30s\n", index, tool.name)
    }
  }

  def loop(): Unit = {
    menu()

    val choice = scala.io.StdIn.readInt()

    choice match {
      case 1 => loop()
      case 2 => loop()
      case 3 => loop()
      case 4 => loop()
      case 5 =>
        list()
        loop()
      case 0 =>
      case _ => loop()
    }
  }

  def main(args: Array[String]): Unit = {
    printf("toolman\n")
    loop()
  }
```

## c

### measure duration in milliseconds

```c
#include <sys/time.h>

int main() {
    struct timeval stop, start;
    gettimeofday(&start, NULL);
    // do something here
    gettimeofday(&stop, NULL);
    printf("%lu\n", stop.tv_usec - start.tv_usec);
}
```

### copy file

```c
int copy_file(const char *src, const char *dst)
{
    int c;
    FILE *fsrc, *fdst;

    fsrc = fopen(src, "rb");
    if (fsrc == NULL)
        return 1;

    fdst = fopen(dst, "wb");
    if (fdst == NULL)
        return 1;

    while ((c = fgetc(fsrc)) != EOF)
        fputc(c, fdst);

    return 0;
}
```

## docker

```bash
# build image
docker build -t NAME .

# run image with shell
docker run -it NAME bash

# stop all containers
docker stop $(docker ps -a -q)

# remove all containers
docker rm $(docker ps -a -q)
```

## git

### add repository to github

```bash
git remote add URL
git pull --alow-unrelated-histories
git push --set-upstream origin master
```

### delete remote branch

```bash
git push origin --delete [BRANCHNAME]
```
