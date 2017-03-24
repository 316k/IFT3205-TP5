function gen(n) {
    for(var max=0; max<n; max++) {
        for(var i=0; i<=max; i++) {

            if(max % 2 == 0)
                console.log(i, max - i);
            else
                console.log(max - i, i);
            
        }
    }

    for(var max=n - 2; max>=0; max--) {
        for(var i=max; i>=0; i--) {

            if(max % 2 == 0)
                console.log(n - i - 1, n - (max - i) - 1);
            else
                console.log(n - (max - i) - 1, n - i - 1);
            
        }
    }
}


gen(8)
