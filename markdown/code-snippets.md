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

### levenshtein

```scala
import scala.math._

object Levenshtein {
   def minimum(i1: Int, i2: Int, i3: Int)=min(min(i1, i2), i3)
   def distance(s1:String, s2:String)={
      val dist=Array.tabulate(s2.length+1, s1.length+1){(j,i)=>if(j==0) i else if (i==0) j else 0}

      for(j<-1 to s2.length; i<-1 to s1.length)
         dist(j)(i)=if(s2(j-1)==s1(i-1)) dist(j-1)(i-1)
	            else minimum(dist(j-1)(i)+1, dist(j)(i-1)+1, dist(j-1)(i-1)+1)

      dist(s2.length)(s1.length)
   }

   def main(args: Array[String]): Unit = {
      printDistance(args(0), args(1))
   }

   def printDistance(s1:String, s2:String) = {
      val d = distance(s1, s2)
      val p = d.toDouble / (s1.length + s2.length).toDouble
      println("%s -> %s : %d, %f".format(s1, s2, d, p))
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
