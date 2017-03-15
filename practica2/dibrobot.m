% Función dibrobot.m
% Dibuja el robot en la localización pasada como argumento([x y tita]'),
% y el color en el parámetro c como carácter (p.e. c='b' es azul)

function dibrobot(location_eje,c)

if nargin<2
    c='b';
end

% Geometría del robot (puede cambiarse para otros tamaños de robot)
largo=0.35;          % lado largo
corto=0.25;         % lado corto
descentre=0.05;     % posición relativa del eje de las ruedas respecto al centro geométrico

trasera_dcha=[-largo;-corto;1];
trasera_izda=[-largo;corto;1];
delantera_dcha=[largo;-corto;1];
delantera_izda=[largo;corto;1];
frontal_robot=[largo;0;1];
tita=location_eje(3);
Hwe=[cos(tita) -sin(tita) location_eje(1);
    sin(tita) cos(tita) location_eje(2)
    0        0         1];
Hec=[eye(2) [descentre;0];
     0   0    1];
puntos=Hwe*Hec*[trasera_izda delantera_izda delantera_dcha trasera_dcha trasera_izda frontal_robot trasera_dcha];
robot=puntos(1:2,:)';
color=[c '-'];
plot(robot(:,1), robot(:,2), color);
axis equal