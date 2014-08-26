package com.play;

import java.util.*;

class NGram {

static List<String> computeNGramShingles(String line, int n) {

     List<String> result = new ArrayList<String>(n);

     String[] circularQueue = new String[n];
     StringTokenizer st = new StringTokenizer(line);

     int index = 0;
     int circularQueueSize = 0;

     StringBuffer strBuf = new StringBuffer();

     while (st.hasMoreElements()) {
         String token = st.nextToken();
         if (circularQueueSize == n)
         {
             strBuf.setLength(0);
             for(int pn = 0; pn < n; pn++)
             {
                if (pn > 0)
                {
                    strBuf.append(" ");
                }
                strBuf.append(circularQueue[(index + pn) % n]);
             }
             result.add(strBuf.toString());
             index = (index + 1) % n;
             circularQueueSize--;
         }
         circularQueue[(index + circularQueueSize) % n] = token;
         if (circularQueueSize < n)
         {
             circularQueueSize++;
         }
     }

     if (circularQueueSize == n)
     {
         strBuf.setLength(0);
         for(int pn = 0; pn < n; pn++)
         {
             if (pn > 0)
             {
                strBuf.append(" ");
             }
             strBuf.append(circularQueue[(index + pn) % n]);
         }
         result.add(strBuf.toString());
     }

     return result;
}

public static void main(String[] args)
{
    System.out.println(computeNGramShingles(args[0], Integer.parseInt(args[1])));
}
}
