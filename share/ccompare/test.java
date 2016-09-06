
class test {

  private static int fib(int n){
    if (n == 0) return 0;
    if (n == 1) return 1;
    return fib(n - 1) + fib(n - 2);
  }

  public static void main(String argv[]){
    long start = System.currentTimeMillis();
    int result = fib(30);
    long finish = System.currentTimeMillis();

    System.out.print("RESULT: ");
    System.out.print(result);
    System.out.print(" [");
    System.out.print((finish-start)*0.001);
    System.out.println("]");

  }
};
