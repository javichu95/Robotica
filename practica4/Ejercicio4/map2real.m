function [ coordenadas ] = map2real( coordMap, mapa )
    %Transforma coordenadas del mapa al mundo real.
    coordenadas = [ coordMap(2)-1, size(mapa,1)-coordMap(1) ];
    coordenadas = (coordenadas - 1)/2;
end

