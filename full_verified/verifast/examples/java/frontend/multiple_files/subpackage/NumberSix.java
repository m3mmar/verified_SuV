package subpackage;

class NumberSix
{
  int getSix()
    //@ requires true;
    //@ ensures result == 6;
  {
    return 6;
  }
}
