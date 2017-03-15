fid = fopen('C:\Users\Jorge\Documents\GitHub\Robotica\practica2\odometria.txt');
fin = fgetl(fid); % Linea descriptiva
figure(1)
i = 1;
while ischar(fin), %Mientras n llegue al final del fichero
    fprintf('ciclo %d\n', i);   %Numero de ciclo
    datos = textscan(fid,'%f %f %f \n',1);  %Se obtienen los datos.
    %Se muestran los datos
    fprintf('x %f ', datos{1});
    fprintf('y %f ', datos{2});
    fprintf('th %f\n', datos{3});
    %Se lee la siguiente linea
    fin = fgetl(fid);
    
    %Se muestran los datos (en centimetros).
    dibrobot([datos{1}/100, datos{2}/100, datos{3}]);
    pause(0.1)
    hold on
    i = i+1;
end
hold off