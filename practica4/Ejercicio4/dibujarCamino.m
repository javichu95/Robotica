function [  ] = dibujarCamino( obstaculos, path, mapa )
%Dibuja un camino en un mapa
    xObs = [];
    yObs = [];
    %Se obtinen las coordenadas de los obstaculos.
    for k = 1:size(obstaculos,1)
        obs = obstaculos(k,:);
        xObs = [ xObs obs(2)];
        yObs = [ yObs (size(mapa,1)-obs(1))];
    end;
    %Se dibuja cada camino.
    for i = 1:size(path,1)
        camino = path(i,:);
        figure(i)
        x = [];
        y = [];
        %Se adaptan los valores obtenidos para pintarlos en un grafico.
        j = 1;
        while j < length(camino)
            y = [ y (size(mapa,1)-camino(j))];
            x = [ x camino(j+1) ];
            j = j+2;
        end;
        %Se dibuja el mapa con el camino.
        plot(x,y,'b-',xObs,yObs,'ro');
        title(strcat('Camino ',num2str(i)));
        hold off
    end;

end

