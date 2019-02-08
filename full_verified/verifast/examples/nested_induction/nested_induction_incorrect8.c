//@ #include "nat.gh"

/*@

lemma void induction(nat n1, nat n2, nat n3)
  requires true;
  ensures  true;
{
  switch(n1)
  {
    case succ(n10):
      switch(n2)
      {
        case succ(n20):
          switch(n3)
          {
            case succ(n30):
              induction(n30, n10, n20); //~
            case zero:
          } 
        case zero:
      }
    case zero:
      switch(n2)
      {
        case succ(n20):
        case zero:
      }
  }
}

@*/