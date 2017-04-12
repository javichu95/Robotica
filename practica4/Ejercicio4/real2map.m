function [ coordMap ] = real2map( coordenadas, mapa )
    %Transforma coordenadas del mundo real a las del mapa.
    coordMap = coordenadas*2 + 1;
    coordMap = [ size(mapa,1)-coordMap(2), coordMap(1)+1 ];
end

