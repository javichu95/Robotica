function [ path ] = encontrarCamino( grid, path, candidatos, objetivo )
%Encuentra el mejor camino hacia el objetivo.
    if(isequal(candidatos(size(candidatos,1),:),objetivo) == 0)
        %Buscamos vecinos apropiados.
        pos = candidatos(size(candidatos,1),:);
        menor = 999999;
        vecinos = [ ];
        
        %Compruena el vecino con menor coste.
        if(grid(pos(1)+1,pos(2)) ~= -1 && pos(1)+2 <= size(grid,1))
            if(grid(pos(1)+2,pos(2)) <= menor)
                menor = grid(pos(1)+2,pos(2));
                vecinos = [ vecinos ; pos(1)+2 pos(2)];
            end;
        end;
        
        if(grid(pos(1)-1,pos(2)) ~= -1 && pos(1)-2 >= 1)
            if(grid(pos(1)-2,pos(2)) <= menor)
                menor = grid(pos(1)-2,pos(2));
                vecinos = [ vecinos ; pos(1)-2 pos(2)];
            end;
        end;
        
        if(grid(pos(1),pos(2)-1) ~= -1 && pos(2)-2 >= 1)
            if(grid(pos(1),pos(2)-2) <= menor)
                menor = grid(pos(1),pos(2)-2);
                vecinos = [ vecinos ; pos(1) pos(2)-2];
            end;
        end;
        
        if(grid(pos(1),pos(2)+1) ~= -1 && pos(2)+2 <= size(grid,2))
            if(grid(pos(1),pos(2)+2) <= menor)
                menor = grid(pos(1),pos(2)+2);
                vecinos = [ vecinos ; pos(1) pos(2)+2];
            end;
        end;
        
        if(menor < grid(pos(1),pos(2)))
            %Para cada vecino con menor coste busca el camino optimo.
            for i = 1:size(vecinos,1)
                vec = vecinos(i,:);
                if(grid(vec(1),vec(2)) == menor)
                    path = encontrarCamino(grid,path,[candidatos;vec],objetivo);
                end;
            end;
        end;
        
    else 
        %Si ha encontrado el objetivo, guarda el camino.
        camino = [];
        for i = 1:size(candidatos,1)
            camino = [ camino candidatos(i,1) candidatos(i,2) ];
        end;
        path = [ path; camino ];
    end;
end

