%%Ejercicio 4
%%Jorge Andrés Galindo

goal = [ 1 0 ];
inicio = [ 7 0 ];
fid = fopen('mapaEj4.txt');
fgetl(fid); %Datos sin importancia del fichero.
datos = fgetl(fid);
mapa = [];
while datos ~= -1
    mapa = [ mapa ; str2num(datos) ];
    datos = fgetl(fid);
end;
fclose(fid);

grid = -2*ones(size(mapa,1),size(mapa,2));
obstaculos = [];
%Se marca los obstaculos.
for i = 1:size(grid,1)
    for j = 1:size(grid,2)
        if(mapa(i,j) == 0)
            grid(i,j) = -1;
            obstaculos = [obstaculos ; i j ];
        end;
    end;
end;

%Se marca el objetivo.
goalMapa = real2map(goal,mapa);
grid(goalMapa(1),goalMapa(2)) = 0;

%Se asignan costes a las casillas.
grid = asignarCostes(grid, goalMapa, 0);

%Se marca el inicio.
robot_cell = real2map(inicio,grid);

%Se obtienen los caminos optimos.
candidatos = [ robot_cell ];

path = encontrarCamino(grid,[],candidatos,goalMapa);

%Se muestran los caminos.
dibujarCamino(obstaculos,path,mapa);