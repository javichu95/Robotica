function [ grid ] = asignarCostes( grid,coordenadas, coste )
    %Asigna costes a las celdas, comprobando que las celdas existan y su coste no sea menor.
    if(grid(coordenadas(1)+1,coordenadas(2)) ~= -1 && coordenadas(1) + 2 <= size(grid,1))
        if(grid(coordenadas(1)+2,coordenadas(2)) < -1 || grid(coordenadas(1)+2,coordenadas(2)) > coste+1)
            grid(coordenadas(1)+2,coordenadas(2)) = coste + 1;
            grid = asignarCostes(grid,[ coordenadas(1)+2,coordenadas(2) ],coste+1);
        end;
    end;
    
    if(grid(coordenadas(1)-1,coordenadas(2)) ~= -1 && coordenadas(1) - 2 >= 1)
        if(grid(coordenadas(1)-2,coordenadas(2)) < -1 || grid(coordenadas(1)-2,coordenadas(2)) > coste + 1)
            grid(coordenadas(1)-2,coordenadas(2)) = coste + 1;
            grid = asignarCostes(grid,[ coordenadas(1)-2,coordenadas(2) ],coste+1);
        end;
    end;
    
    if(grid(coordenadas(1),coordenadas(2)+1) ~= -1 && coordenadas(2) + 2 <= size(grid,2))
        if(grid(coordenadas(1),coordenadas(2)+2) < -1 || grid(coordenadas(1),coordenadas(2)+2) > coste+1)
            grid(coordenadas(1),coordenadas(2)+2) = coste + 1;
            grid = asignarCostes(grid,[ coordenadas(1),coordenadas(2)+2 ],coste+1);
        end;
    end;
    
    if(grid(coordenadas(1),coordenadas(2)-1) ~= -1 && coordenadas(2) - 2 >= 1)
        if(grid(coordenadas(1),coordenadas(2)-2) < -1 || grid(coordenadas(1),coordenadas(2)-2) > coste+1)
            grid(coordenadas(1),coordenadas(2)-2) = coste + 1;
            grid = asignarCostes(grid,[ coordenadas(1),coordenadas(2)-2 ],coste+1);
        end;
    end;
end

