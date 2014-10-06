USE siga;

DESC TBL_GRADE;
DESC TBL_GRADE_ITEM;
DESC TBL_GRADE_ITEM_PERIODO;
DESC TBL_GRADE_ITEM_DISCIPLINA;
DESC TBL_GRADE_ITEM_DISCIPLINA_REQUISITO;
DESC TBL_GRADE_ITEM_DISCIPLINA_REQUERIMENTO;
DESC TBL_FUNCIONARIO;

SELECT
	#grd_codigo
	grdd_sigla, dsp_titulo, grdd_carga_horaria_acumulada, grdi_cod_GRADE_ITEM_PERIODO, cur_titulo, grdd_pre_requisito
	#DISTINCT fun_codigo, fun_nome
	#fun_codigo, grdd_sigla
FROM
	(
		SELECT
			grd_codigo, 

			grdd_codigo, grdg_codigo, 

			reflet_codigo, reflet_descricao, 


			grdd_sigla, dsp_titulo, grdd_carga_horaria_acumulada, grdi_cod_GRADE_ITEM_PERIODO, cur_titulo,
			(
				SELECT
					GROUP_CONCAT(PR.grdd_sigla)
				FROM
					TBL_GRADE_ITEM_DISCIPLINA_REQUISITO
					JOIN TBL_GRADE_ITEM_DISCIPLINA PR ON (grdr_requisito_cod_GRADE_ITEM_DISCIPLINA = PR.grdd_codigo)
				WHERE
					grdr_referencia_cod_GRADE_ITEM_DISCIPLINA = TBL_GRADE_ITEM_DISCIPLINA.grdd_codigo
			) as grdd_pre_requisito,

			fun_codigo, fun_nome
		FROM
			TBL_GRADE
			JOIN TBL_CURSO ON (grd_cod_CURSO = cur_codigo)
			JOIN TBL_GRADE_ITEM ON (grd_codigo = grdi_cod_GRADE)
			JOIN TBL_GRADE_ITEM_DISCIPLINA ON (grdi_codigo = grdd_cod_GRADE_ITEM)
			JOIN TBL_DISCIPLINA ON (grdd_cod_DISCIPLINA = dsp_codigo)
			JOIN TBL_GRADE_ITEM_DISCIPLINA_REQUERIMENTO ON (grdd_codigo = grdg_cod_GRADE_ITEM_DISCIPLINA)
			JOIN TBL_FUNCIONARIO ON (grdg_cod_FUNCIONARIO = fun_codigo)
			JOIN TBL_REFERENCIA_LETIVA ON (grdg_cod_REFERENCIA_LETIVA = reflet_codigo)
		ORDER BY
			grdd_sigla, reflet_descricao DESC
	) AS tmp
WHERE
	grd_codigo = 35
GROUP BY 
	grdd_codigo
;


SELECT
	mtr_codigo, COUNT(*), dsp_restante, dsp_cursadas
FROM
	(
		SELECT
			mtr_codigo, mtr_cod_GRADE, mtr_grade_especial, mtr_ativo
			, (
				SELECT 
					GROUP_CONCAT(grdd_sigla)
				FROM 
					TBL_REQUERIMENTO
					JOIN TBL_REQUERIMENTO_ITEM ON (req_codigo = reqi_cod_REQUERIMENTO) 
					JOIN TBL_GRADE_ITEM_DISCIPLINA ON (reqi_cod_GRADE_ITEM_DISCIPLINA = grdd_codigo)
					JOIN TBL_GRADE_ITEM ON (grdd_cod_GRADE_ITEM = grdi_codigo)
				WHERE 
					grdd_codigo NOT IN (
						SELECT 
							grdds_cod_GRADE_ITEM_DISCIPLINA 
						FROM 
							TBL_GRADE_ITEM_DISCIPLINA_DISPENSA 
						WHERE 
							grdds_cod_MATRICULA = mtr_codigo
					)
					AND req_cod_MATRICULA = mtr_codigo
					AND reqi_resultado <> 'APROVADO'
			) AS dsp_restante
			, (
				SELECT
					GROUP_CONCAT(grdd_sigla)
				FROM
					TBL_REQUERIMENTO
					JOIN TBL_REQUERIMENTO_ITEM ON (req_codigo = reqi_cod_REQUERIMENTO)
					JOIN TBL_GRADE_ITEM_DISCIPLINA ON (reqi_cod_GRADE_ITEM_DISCIPLINA = grdd_codigo)
					JOIN TBL_GRADE_ITEM_DISCIPLINA_REQUISITO ON (grdd_codigo = grdr_requisito_cod_GRADE_ITEM_DISCIPLINA)
				WHERE
					req_cod_MATRICULA = mtr_codigo
			) AS dsp_cursadas
		FROM
			TBL_MATRICULA
		ORDER BY
			mtr_cod_GRADE, mtr_grade_especial
	) AS tmp
WHERE
	mtr_cod_GRADE = 35 AND mtr_ativo
GROUP BY
	dsp_restante, dsp_cursadas
;